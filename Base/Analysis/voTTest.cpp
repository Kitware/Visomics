
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voTTest.h"
#include "voTableDataObject.h"
#include "vtkExtendedTable.h"
#include "voUtils.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voTTestPrivate methods

// --------------------------------------------------------------------------
class voTTestPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voTTest methods

// --------------------------------------------------------------------------
voTTest::voTTest():
    Superclass(), d_ptr(new voTTestPrivate)
{
  Q_D(voTTest);

  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}

// --------------------------------------------------------------------------
voTTest::~voTTest()
{
}

// --------------------------------------------------------------------------
void voTTest::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voTTest::setOutputInformation()
{
  this->addOutputType("TTest_table", "vtkTable" ,
                      "", "",
                      "voTableView", "Table");
}

// --------------------------------------------------------------------------
void voTTest::setParameterInformation()
{
  QList<QtProperty*> TTest_parameters;

  TTest_parameters << this->addStringParameter("sample1_range", QObject::tr("Sample Group 1"), "A-C,F");
  TTest_parameters << this->addStringParameter("sample2_range", QObject::tr("Sample Group 2"), "D,E,G-J");

  this->addParameterGroup("T-Test parameters", TTest_parameters);

}

// --------------------------------------------------------------------------
bool voTTest::execute()
{
  Q_D(voTTest);

  // Get and parse parameters
  bool result;

  QList<int> sample1RangeList;
  result = voUtils::parseRangeString(this->stringParameter("sample1_range"), sample1RangeList, true);
  if(!result || sample1RangeList.isEmpty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse range list: Initial Sample(s)");
    return false;
    }

  QList<int> sample2RangeList;
  result= voUtils::parseRangeString(this->stringParameter("sample2_range"), sample2RangeList, true);
  if(!result || sample1RangeList.isEmpty())
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
  vtkSmartPointer<vtkArrayData> RInputArrayData = vtkSmartPointer<vtkArrayData>::New();
  RInputArrayData->AddArray(sample1Array);
  RInputArrayData->AddArray(sample2Array);

  // Run R code
  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputConnection(RInputArrayData->GetProducerPort());
  d->RCalc->PutArray("0", "sample1Array");
  d->RCalc->PutArray("1", "sample2Array");
  d->RCalc->GetArray("P-Value","pValue");
  d->RCalc->GetArray("RerrValue","RerrValue");
  d->RCalc->SetRscript(
  "constErrFlag <- 0; genErrFlag <- 0\n"
  "my.t.test<-function(...){"
    "obj<-try(t.test(...), silent=TRUE) \n"
    "if( ! is(obj, \"try-error\") ){"
      "return(obj$p.value)"
    "}else if ( length(grep(\"data are essentially constant\", geterrmessage(), fixed=TRUE)) > 0 ){"
      "constErrFlag <<- 1\n"
    "}else{"
      "genErrFlag <<- 1}\n"
      "return(NA)}\n"
  "pValue <- sapply( seq(length=nrow(sample1Array)), "
                    "function(x) {my.t.test(sample1Array[x,], sample2Array[x,], \"two.sided\")})\n"
  "if(genErrFlag){"
    "RerrValue <- 2"
  "}else if(constErrFlag){"
    "RerrValue <- 1"
  "}else{"
    "RerrValue <- 0}\n");
  d->RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());

  // Check for errors "thrown" by R script
  if(outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() == 1)
    {
    qWarning() << QObject::tr("T-Test warning: data are essentially constant");
    }
  else if(outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() > 1)
    {
    qWarning() << QObject::tr("Fatal error in T-Test R script");
    return false;
    }

  // Extract and build table for p-values
  vtkSmartPointer<vtkTable> pValueTable = vtkSmartPointer<vtkTable>::New();
  voUtils::arrayToTable(outputArrayData->GetArrayByName("P-Value"), pValueTable.GetPointer());

  // Combine tables
  vtkSmartPointer<vtkTable> outputDataTable = vtkSmartPointer<vtkTable>::New();
  voUtils::insertColumnIntoTable(outputDataTable.GetPointer(), 0, extendedTable->GetRowMetaDataOfInterest());
  outputDataTable->AddColumn(pValueTable->GetColumn(0));

  this->setOutput("TTest_table", new voTableDataObject("TTest_table", outputDataTable));
  return true;
}
