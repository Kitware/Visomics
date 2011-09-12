/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

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
                      "voTableView", "Significance (Table)");

  this->addOutputType("TTest_volcano", "vtkTable" ,
                      "voVolcanoView", "Volcano Plot",
                      "voTableView", "Volcano Table");
}

// --------------------------------------------------------------------------
void voTTest::setParameterInformation()
{
  QList<QtProperty*> ttest_parameters;

  ttest_parameters << this->addStringParameter("sample1_range", QObject::tr("Sample Group 1"), "A-C,F");
  ttest_parameters << this->addStringParameter("sample2_range", QObject::tr("Sample Group 2"), "D,E,G-J");

  this->addParameterGroup("T-Test parameters", ttest_parameters);
}

// --------------------------------------------------------------------------
QString voTTest::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Sample Group 1 / 2</b>:</dt>"
                 "<dd>A group of Experiments, specified by a range and/or list of column letters.</dd>"
                 "</dl>");
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
  vtkSmartPointer<vtkArrayData> RInputArrayData = vtkSmartPointer<vtkArrayData>::New();
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
  "log2FC<-log2(rowMeans(sample1Array))-log2(rowMeans(sample2Array))\n"
  "FCFun <- function(x){"
    "if(is.na(x)){genErrFlag <<- 1; return(0)}\n"
    "if (x<0) {return(-1/(2^x))} else {return(2^x)}}\n"
  "foldChange<-sapply(log2FC, FCFun)\n"
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

  // Extract and build table for fold change
  vtkSmartPointer<vtkTable> foldChangeTable = vtkSmartPointer<vtkTable>::New();
  voUtils::arrayToTable(outputArrayData->GetArrayByName("Fold Change (Sample 1 -> Sample 2)"), foldChangeTable.GetPointer());

  // Combine tables
  vtkSmartPointer<vtkTable> outputDataTable = vtkSmartPointer<vtkTable>::New();
  voUtils::insertColumnIntoTable(outputDataTable.GetPointer(), 0, extendedTable->GetRowMetaDataOfInterest());
  outputDataTable->AddColumn(pValueTable->GetColumn(0));

  vtkSmartPointer<vtkTable> outputVolcanoTable = vtkSmartPointer<vtkTable>::New();
  voUtils::insertColumnIntoTable(outputVolcanoTable.GetPointer(), 0, extendedTable->GetRowMetaDataOfInterest());
  outputVolcanoTable->AddColumn(foldChangeTable->GetColumn(0));
  outputVolcanoTable->AddColumn(pValueTable->GetColumn(0));

  this->setOutput("TTest_table", new voTableDataObject("TTest_table", outputDataTable));
  this->setOutput("TTest_volcano", new voTableDataObject("TTest_volcano", outputVolcanoTable));
  return true;
}
