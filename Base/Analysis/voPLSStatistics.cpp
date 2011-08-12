
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voPLSStatistics.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkDoubleArray.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkNew.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voPLSStatisticsPrivate methods

// --------------------------------------------------------------------------
class voPLSStatisticsPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voPLSStatistics methods

// --------------------------------------------------------------------------
voPLSStatistics::voPLSStatistics():
    Superclass(), d_ptr(new voPLSStatisticsPrivate)
{
  Q_D(voPLSStatistics);
  
  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}

// --------------------------------------------------------------------------
voPLSStatistics::~voPLSStatistics()
{
}

// --------------------------------------------------------------------------
void voPLSStatistics::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voPLSStatistics::setOutputInformation()
{
  this->addOutputType("scores", "vtkTable" ,
                      "", "",
                      "voTableView", "Table (Scores)");
}

// --------------------------------------------------------------------------
void voPLSStatistics::setParameterInformation()
{
  QList<QtProperty*> PLS_parameters;

  PLS_parameters << this->addStringParameter("predictor_range", QObject::tr("Predictor Analyte(s)"), "1-3,6");
  PLS_parameters << this->addStringParameter("response_range", QObject::tr("Response Analyte(s)"), "4,5,7-10");
  PLS_parameters << this->addEnumParameter("algorithm", tr("Algorithm"), 
                            (QStringList() << "Kernel" << "Wide Kernel" << "SIMPLS" << "Orthogonal Scores"), 
                            "Kernel");

  this->addParameterGroup("PLS parameters", PLS_parameters);

}

// --------------------------------------------------------------------------
bool voPLSStatistics::execute()
{
  Q_D(voPLSStatistics);

  // Get and parse parameters
  QString algorithmString;
  if (this->enumParameter("algorithm") == QString("Kernel"))
    {
    algorithmString = "kernelpls";
    }
  else if (this->enumParameter("algorithm") == QString("Wide Kernel"))
    {
    algorithmString = "widekernelpls";
    }
  else if (this->enumParameter("algorithm") == QString("SIMPLS"))
    {
    algorithmString = "simpls";
    }
  else //if (this->enumParameter("algorithm") == QString("Orthogonal Scores"))
    {
    algorithmString = "oscorespls";
    }

  bool result;

  QList<int> predictorRangeList;
  result = voUtils::parseRangeString(this->stringParameter("predictor_range"), predictorRangeList, false);
  if(!result || predictorRangeList.empty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse Predictor Measure(s)");
    return false;
    }

  QList<int> responseRangeList;
  result= voUtils::parseRangeString(this->stringParameter("response_range"), responseRangeList, false);
  if(!result || responseRangeList.empty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse Response Measure(s)");
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
  // PLS expects each measure (analyte) as a column, and each sample (experiment) as a row, so we must transpose
  vtkNew<vtkTable> inputDataTableTransposed;
  bool transposeResult = voUtils::transposeTable(inputDataTable.GetPointer(), inputDataTableTransposed.GetPointer());
  if (!transposeResult)
    {
    qWarning() << QObject::tr("Error: could not transpose input table");
    return false;
    }

  // Build array for predictor measure range
  vtkSmartPointer<vtkArray> predictorArray;
  bool tabToArrResult = voUtils::tableToArray(inputDataTableTransposed.GetPointer(), predictorArray, predictorRangeList);
  if (!tabToArrResult)
    {
    qWarning() << QObject::tr("Invalid paramater, out of range: Predictor Measure(s)");
    return false;
    }

  // Build array for response measure
  vtkSmartPointer<vtkArray> responseArray;
  tabToArrResult = voUtils::tableToArray(inputDataTableTransposed.GetPointer(), responseArray, responseRangeList);
  if (!tabToArrResult)
    {
    qWarning() << QObject::tr("Invalid paramater, out of range: Response Measure(s)");
    return false;
    }

  // Combine sample 1 and 2 array groups
  vtkNew<vtkArrayData> RInputArrayData;
  RInputArrayData->AddArray(predictorArray.GetPointer());
  RInputArrayData->AddArray(responseArray.GetPointer());

  // Run R code
  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputConnection(RInputArrayData->GetProducerPort());
  d->RCalc->PutArray("0", "predictorArray");
  d->RCalc->PutArray("1", "responseArray");
  d->RCalc->GetArray("scoresArray","scoresArray");
  d->RCalc->GetArray("RerrValue","RerrValue");
  d->RCalc->SetRscript(QString(
  "library(\"pls\", warn.conflicts=FALSE)\n"
  "PLSdata <- data.frame(response=I(responseArray), predictor=I(predictorArray) )\n"
  "PLSresult <- plsr(response ~ predictor, data = PLSdata, method = \"%1\")\n"
  "if(exists(\"PLSresult\")) {"
    "RerrValue<-1"
  "}else{"
    "RerrValue<-0"
  "}\n"
  "scoresArray <- t(PLSresult$scores)\n"
  ).arg(algorithmString).toLatin1());
  d->RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());

  // Check for errors "thrown" by R script
  if(outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() > 1)
    {
    qWarning() << QObject::tr("Error: R could not calculate PLS");
    return false;
    }

  // Extract table for scores
  // No need to transpose scoresArray, it was done within the R code
  vtkNew<vtkTable> scoresTable;
  voUtils::arrayToTable(outputArrayData->GetArrayByName("scoresArray"), scoresTable.GetPointer());
  // Add column labels (experiment names)
  voUtils::setTableColumnNames(scoresTable.GetPointer(), extendedTable->GetColumnMetaDataOfInterestAsString());
  // Add row labels (components)
  vtkNew<vtkStringArray> scoresHeaderArr;
  for (vtkIdType r = 0;r < scoresTable->GetNumberOfRows(); ++r)
    {
    scoresHeaderArr->InsertNextValue(QString("Comp %1").arg(r + 1).toLatin1());
    }
  voUtils::insertColumnIntoTable(scoresTable.GetPointer(), 0, scoresHeaderArr.GetPointer());

  this->setOutput("scores", new voTableDataObject("scores", scoresTable.GetPointer()));

  return true;
}
