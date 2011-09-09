
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voANOVAStatistics.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voANOVAStatisticsPrivate methods

// --------------------------------------------------------------------------
class voANOVAStatisticsPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voANOVAStatistics methods

// --------------------------------------------------------------------------
voANOVAStatistics::voANOVAStatistics():
    Superclass(), d_ptr(new voANOVAStatisticsPrivate)
{
  Q_D(voANOVAStatistics);
  
  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}

// --------------------------------------------------------------------------
voANOVAStatistics::~voANOVAStatistics()
{
}

// --------------------------------------------------------------------------
void voANOVAStatistics::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voANOVAStatistics::setOutputInformation()
{    
  this->addOutputType("ANOVA_table", "vtkTable" ,
                      "", "",
                      "voTableView", "Significance (Table)");

  this->addOutputType("ANOVA_volcano", "vtkTable" ,
                      "voVolcanoView", "Volcano Plot",
                      "voTableView", "Volcano Table");
}

// --------------------------------------------------------------------------
void voANOVAStatistics::setParameterInformation()
{
  QList<QtProperty*> ANOVA_parameters;

  ANOVA_parameters << this->addStringParameter("sample1_range", QObject::tr("Sample Group 1"), "A-C,F");
  ANOVA_parameters << this->addStringParameter("sample2_range", QObject::tr("Sample Group 2"), "D,G-I");

  this->addParameterGroup("ANOVA parameters", ANOVA_parameters);
}

// --------------------------------------------------------------------------
QString voANOVAStatistics::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Sample Group 1 / 2</b>:</dt>"
                 "<dd>A group of Experiments, specified by a range and/or list of column letters.<br>"
                 "<u>The groups MUST both be the same size</u>, as only a balanced test is performed</dd>"
                 "</dl>");
}

// --------------------------------------------------------------------------
bool voANOVAStatistics::execute()
{
  Q_D(voANOVAStatistics);

  // Get and parse parameters
  bool result;

  QList<int> sample1RangeList;
  result = voUtils::parseRangeString(this->stringParameter("sample1_range"), sample1RangeList, true);
  if(!result || sample1RangeList.isEmpty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse range list: Sample Group 1");
    return false;
    }

  QList<int> sample2RangeList;
  result= voUtils::parseRangeString(this->stringParameter("sample2_range"), sample2RangeList, true);
  if(!result || sample1RangeList.isEmpty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse range list: Sample Group 2");
    return false;
    }

  if(sample1RangeList.size() != sample2RangeList.size())
    {
    qWarning() << QObject::tr("Invalid paramaters, Sample Groups must be the same size (balanced)");
    return false;
    }

  // Import data table locally
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
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
    qWarning() << QObject::tr("Invalid paramater, out of range: Sample Group 1");
    return false;
    }

  // Build array for sample 2 data range
  vtkSmartPointer<vtkArray> sample2Array;
  result = voUtils::tableToArray(inputDataTable.GetPointer(), sample2Array, sample2RangeList);
  if (!result)
    {
    qWarning() << QObject::tr("Invalid paramater, out of range: Sample Group 2");
    return false;
    }

  // Combine sample 1 and 2 array groups
  vtkNew<vtkArrayData> RInputArrayData;
  RInputArrayData->AddArray(sample1Array);
  RInputArrayData->AddArray(sample2Array);

  // Run R code
  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputConnection(RInputArrayData->GetProducerPort());
  d->RCalc->PutArray("0", "sample1Array");
  d->RCalc->PutArray("1", "sample2Array");
  d->RCalc->GetArray("P-Value","pValue");
  d->RCalc->GetArray("Fold Change (Sample 1 -> Sample 2)","foldChange");
  d->RCalc->GetArray("RerrValue","RerrValue");
  d->RCalc->SetRscript(
  "genErrFlag <- 0\n"
  "pValue <- sapply( seq(length=nrow(sample1Array)), "
                    "function(x) {summary(aov(sample1Array[x,] ~ sample2Array[x,]))[[1]][[1,\"Pr(>F)\"]] })\n"
  "log2FC<-log2(rowMeans(sample1Array))-log2(rowMeans(sample2Array))\n"
  "FCFun <- function(x){"
    "if(is.na(x)){genErrFlag <<- 1; return(0)}\n"
    "if (x<0) {return(-1/(2^x))} else {return(2^x)}}\n"
  "foldChange<-sapply(log2FC, FCFun)\n"
  "if(genErrFlag){"
    "RerrValue <- 2"
  "}else{"
    "RerrValue <- 0}\n");
  d->RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());

  // Check for errors "thrown" by R script
  if(!outputArrayData || outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() > 1)
    {
    qCritical() << QObject::tr("Fatal error in ANOVA R script");
    return false;
    }

  // Extract and build table for p-values
  vtkNew<vtkTable> pValueTable;
  voUtils::arrayToTable(outputArrayData->GetArrayByName("P-Value"), pValueTable.GetPointer());

  // Extract and build table for fold change
  vtkNew<vtkTable> foldChangeTable;
  voUtils::arrayToTable(outputArrayData->GetArrayByName("Fold Change (Sample 1 -> Sample 2)"), foldChangeTable.GetPointer());

  // Combine tables
  vtkNew<vtkTable> outputDataTable;
  voUtils::insertColumnIntoTable(outputDataTable.GetPointer(), 0, extendedTable->GetRowMetaDataOfInterest());
  outputDataTable->AddColumn(pValueTable->GetColumn(0));

  vtkNew<vtkTable> outputVolcanoTable;
  voUtils::insertColumnIntoTable(outputVolcanoTable.GetPointer(), 0, extendedTable->GetRowMetaDataOfInterest());
  outputVolcanoTable->AddColumn(foldChangeTable->GetColumn(0));
  outputVolcanoTable->AddColumn(pValueTable->GetColumn(0));

  this->setOutput("ANOVA_table", new voTableDataObject("ANOVA_table", outputDataTable.GetPointer()));
  this->setOutput("ANOVA_volcano", new voTableDataObject("ANOVA_volcano", outputVolcanoTable.GetPointer()));
  return true;
}
