
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voFoldChange.h"
#include "voTableDataObject.h"
#include "vtkExtendedTable.h"
#include "voUtils.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voFoldChangePrivate methods

// --------------------------------------------------------------------------
class voFoldChangePrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voFoldChange methods

// --------------------------------------------------------------------------
voFoldChange::voFoldChange():
    Superclass(), d_ptr(new voFoldChangePrivate)
{
  Q_D(voFoldChange);
  
  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}

// --------------------------------------------------------------------------
voFoldChange::~voFoldChange()
{
}

// --------------------------------------------------------------------------
void voFoldChange::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voFoldChange::setOutputInformation()
{    
  this->addOutputType("foldChange", "vtkTable" ,
                      "", "",
                      "voTableView", "Table");
  this->addOutputType("foldChangePlot", "vtkTable" ,
                      "voHorizontalBarView", "Horizontal Plot",
                      "", "");
}

// --------------------------------------------------------------------------
void voFoldChange::setParameterInformation()
{
  QList<QtProperty*> fold_change_parameters;

  fold_change_parameters << this->addEnumParameter("mean_method", tr("Mean Method"), 
                                    (QStringList() << "Geometric" << "Arithmetic"), "Arithmetic");
  fold_change_parameters << this->addStringParameter("sample1_range", QObject::tr("Initial Sample(s)"), "A-C,F");
  fold_change_parameters << this->addStringParameter("sample2_range", QObject::tr("Final Sample(s)"), "D,E,G-J");

  this->addParameterGroup("Fold Change parameters", fold_change_parameters);

}

// --------------------------------------------------------------------------
bool voFoldChange::execute()
{
  Q_D(voFoldChange);

  // Get and parse parameters
  bool result;

  int meanMethod = (this->enumParameter("mean_method") == QString("Geometric")) ? 0 : 1; // 0 for geo, 1 for arith
  QList<int> sample1RangeList;
  result = voUtils::parseRangeString(this->stringParameter("sample1_range"), sample1RangeList, true);
  if(!result || sample1RangeList.isEmpty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse range list: Initial Sample(s)");
    return false;
    }

  QList<int> sample2RangeList;
  result= voUtils::parseRangeString(this->stringParameter("sample2_range"), sample2RangeList, true);
  if(!result || sample2RangeList.isEmpty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse range list: Final Sample(s)");
    return false;
    }

  // Import data table locally
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->data());
  if (!extendedTable)
    {
    qWarning() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> inputDataTable = extendedTable->GetData();

  // Build array for sample 1 data range
  vtkSmartPointer<vtkArray> sample1Array;
  result = voUtils::tableToArray(inputDataTable.GetPointer(), sample1Array, sample1RangeList);
  if (!result)
    {
    qWarning() << QObject::tr("Invalid paramater, out of range: Initial Sample(s)");
    return false;
    }

  // Build array for sample 2 data range
  vtkSmartPointer<vtkArray> sample2Array;
  result = voUtils::tableToArray(inputDataTable.GetPointer(), sample2Array, sample2RangeList);
  if (!result)
    {
    qWarning() << QObject::tr("Invalid paramater, out of range: Final Sample(s)");
    return false;
    }

  // Combine sample 1 and 2 array groups
  vtkSmartPointer<vtkArrayData> RInputArrayData = vtkSmartPointer<vtkArrayData>::New();
  RInputArrayData->AddArray(sample1Array);
  RInputArrayData->AddArray(sample2Array);

  // Run R code
  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputConnection(RInputArrayData->GetProducerPort());
  d->RCalc->PutArray("0", "sample1Array");
  d->RCalc->PutArray("1", "sample2Array");
  d->RCalc->GetArray("Average Initial","avgInit");
  d->RCalc->GetArray("Average Final","avgFinal");
  d->RCalc->GetArray("Fold Change","foldChange");
  d->RCalc->GetArray("RerrValue","RerrValue");
  d->RCalc->SetRscript(QString(
  "RerrValue<-0; meanMethod<- %1 \n"
  "if(meanMethod == 0) {"
    "avgInit<-2^rowMeans(log2(sample1Array))\n"
    "avgFinal<-2^rowMeans(log2(sample2Array))\n"
  "}else{"
    "avgInit<-rowMeans(sample1Array)\n"
    "avgFinal<-rowMeans(sample2Array) }\n"
  "log2FC<-(log2(avgFinal)-log2(avgInit))\n"
  "FCFun <- function(x){"
    "if(is.na(x)){RerrValue <<- 2; return(0)}\n"
    "if (x<0) {return(-1/(2^x))} else {return(2^x)}}\n"
  "foldChange<-sapply(log2FC, FCFun)"
  ).arg(meanMethod).toLatin1());
  d->RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());

  // Check for errors "thrown" by R script
  if(outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() > 1)
    {
    qWarning() << QObject::tr("Fold change warning: cannot calculate from zero or negative input");
    //return false;
    }

  // Extract and build table for average initial value
  vtkSmartPointer<vtkTable> avgInitTable= vtkSmartPointer<vtkTable>::New();
  voUtils::arrayToTable(outputArrayData->GetArrayByName("Average Initial"), avgInitTable.GetPointer());

  // Extract and build table for average final value
  vtkSmartPointer<vtkTable> avgFinalTable= vtkSmartPointer<vtkTable>::New();
  voUtils::arrayToTable(outputArrayData->GetArrayByName("Average Final"), avgFinalTable.GetPointer());

  // Extract and build table for fold change
  vtkSmartPointer<vtkTable> foldChangeTable= vtkSmartPointer<vtkTable>::New();
  voUtils::arrayToTable(outputArrayData->GetArrayByName("Fold Change"), foldChangeTable.GetPointer());

  // Combine tables
  vtkSmartPointer<vtkTable> outputDataTable = vtkSmartPointer<vtkTable>::New();
  voUtils::insertColumnIntoTable(outputDataTable.GetPointer(), 0, extendedTable->GetRowMetaDataOfInterest());
  outputDataTable->AddColumn(avgInitTable->GetColumn(0));
  outputDataTable->AddColumn(avgFinalTable->GetColumn(0));
  outputDataTable->AddColumn(foldChangeTable->GetColumn(0));

  vtkSmartPointer<vtkTable> outputPlotTable = vtkSmartPointer<vtkTable>::New();
  voUtils::insertColumnIntoTable(outputPlotTable.GetPointer(), 0, extendedTable->GetRowMetaDataOfInterest());
  outputPlotTable->AddColumn(foldChangeTable->GetColumn(0));

  this->setOutput("foldChange", new voTableDataObject("foldChange", outputDataTable));
  this->setOutput("foldChangePlot", new voTableDataObject("foldChangePlot", outputPlotTable));
  return true;
}
