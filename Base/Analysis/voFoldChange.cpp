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
#include <QList>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voFoldChange.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
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
                      "voTableView", "Change (Table)");

  this->addOutputType("foldChangePlot", "vtkTable" ,
                      "voHorizontalBarView", "Change (Plot)",
                      "", "");
}

// --------------------------------------------------------------------------
void voFoldChange::setParameterInformation()
{
  QList<QtProperty*> fold_change_parameters;

  fold_change_parameters << this->addEnumParameter("mean_method", tr("Mean Method"), 
                                    (QStringList() <<  "Arithmetic" << "Geometric"), "Arithmetic");
  fold_change_parameters << this->addStringParameter("sample1_range", QObject::tr("Initial Sample(s)"), "A-C,F");
  fold_change_parameters << this->addStringParameter("sample2_range", QObject::tr("Final Sample(s)"), "D,E,G-J");

  this->addParameterGroup("Fold Change parameters", fold_change_parameters);
}

// --------------------------------------------------------------------------
QString voFoldChange::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Initial / Final Sample(s)</b>:</dt>"
                 "<dd>A group of Experiments, specified by a range and/or list of column letters.</dd>"
                 "</dl>");
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
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qCritical() << "Input is Null";
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
  vtkNew<vtkArrayData> RInputArrayData;
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
    "if(!is.finite(x)){RerrValue <<- 2; return(-NaN)}\n"
    "if (x<0) {return(-1/(2^x))} else {return(2^x)}}\n"
  "foldChange<-sapply(log2FC, FCFun)"
  ).arg(meanMethod).toLatin1());
  d->RCalc->Update();

  // Get R output
  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());
  if(!outputArrayData)
    {
    qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
    return false;
    }
  // Check for errors "thrown" by R script
  if(outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() > 1)
    {
    qWarning() << QObject::tr("Fold change warning: cannot calculate fold change from zero or negative input");
    }

  // Get analyte names
  vtkSmartPointer<vtkStringArray> analyteNames = extendedTable->GetRowMetaDataOfInterestAsString();

  // Build table with names and fold change only
  vtkNew<vtkTable> outputPlotTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("Fold Change"), outputPlotTable.GetPointer());
    voUtils::insertColumnIntoTable(outputPlotTable.GetPointer(), 0, analyteNames.GetPointer());
    }

  vtkNew<vtkTable> outputDataTable;
    {
    outputDataTable->DeepCopy(outputPlotTable.GetPointer());

    vtkNew<vtkTable> tempAvgInitTable;
    voUtils::arrayToTable(outputArrayData->GetArrayByName("Average Initial"), tempAvgInitTable.GetPointer());
    voUtils::insertColumnIntoTable(outputDataTable.GetPointer(), 1, tempAvgInitTable->GetColumn(0));

    vtkNew<vtkTable> tempAvgFinalTable;
    voUtils::arrayToTable(outputArrayData->GetArrayByName("Average Final"), tempAvgFinalTable.GetPointer());
    voUtils::insertColumnIntoTable(outputDataTable.GetPointer(), 2, tempAvgFinalTable->GetColumn(0));
    }

  this->setOutput("foldChange", new voTableDataObject("foldChange", outputDataTable.GetPointer()));
  this->setOutput("foldChangePlot", new voTableDataObject("foldChangePlot", outputPlotTable.GetPointer()));
  return true;
}
