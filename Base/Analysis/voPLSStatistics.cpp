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
#include "voPLSStatistics.h"
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
                      "voPCAProjectionView", "Scores (Plot)",
                      "voTableView", "Scores (Table)");

  this->addOutputType("yScores", "vtkTable" ,
                      "voPCAProjectionView", "Y-Scores (Plot)",
                      "voTableView", "Y-Scores (Plot)");

  this->addOutputType("loadings", "vtkTable" ,
                      "", "",
                      "voTableView", "Loadings (Table)");

  this->addOutputType("loadings_transposed", "vtkTable" ,
                      "voPCAProjectionView", "Loadings (Plot)",
                      "", "");

  this->addOutputType("loadingWeights", "vtkTable" ,
                      "", "",
                      "voTableView", "Loading Weights (Table)");

  this->addOutputType("loadingWeights_transposed", "vtkTable" ,
                      "voPCAProjectionView", "Loading Weights (Plot)",
                      "", "");

  this->addOutputType("yLoadings", "vtkTable" ,
                      "", "",
                      "voTableView", "Y-Loadings (Table)");

  this->addOutputType("yLoadings_transposed", "vtkTable" ,
                      "voPCAProjectionView", "Y-Loadings (Plot)",
                      "", "");
}

// --------------------------------------------------------------------------
void voPLSStatistics::setParameterInformation()
{
  QList<QtProperty*> pls_parameters;

  pls_parameters << this->addStringParameter("predictor_range", QObject::tr("Predictor Analyte(s)"), "1-3,6");
  pls_parameters << this->addStringParameter("response_range", QObject::tr("Response Analyte(s)"), "4,5,7-10");
  pls_parameters << this->addEnumParameter("algorithm", tr("Algorithm"),
                            (QStringList() << "Kernel" << "Wide Kernel" << "SIMPLS" << "Orthogonal Scores"),
                            "Kernel");

  this->addParameterGroup("PLS parameters", pls_parameters);
}

// --------------------------------------------------------------------------
QString voPLSStatistics::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Predictor / Response Analyte(s)</b>:</dt>"
                 "<dd>A group of Analytes, specified by a range and/or list of row numbers.</dd>"
                 "<dt><b>Algorithm</b>:</dt>"
                 "<dd>The multivariante regression method.</dd>"
                 "</dl>");
}

// --------------------------------------------------------------------------
bool voPLSStatistics::execute()
{
  Q_D(voPLSStatistics);

  // Get and parse parameters
  QString algorithmString;
  if (this->enumParameter("algorithm") == QLatin1String("Kernel"))
    {
    algorithmString = "kernelpls";
    }
  else if (this->enumParameter("algorithm") == QLatin1String("Wide Kernel"))
    {
    algorithmString = "widekernelpls";
    }
  else if (this->enumParameter("algorithm") == QLatin1String("SIMPLS"))
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
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qCritical() << "Input is Null";
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
  d->RCalc->SetInputData(RInputArrayData.GetPointer());
  d->RCalc->PutArray("0", "predictorArray");
  d->RCalc->PutArray("1", "responseArray");
  d->RCalc->GetArray("scoresArray","scoresArray");
  d->RCalc->GetArray("yScoresArray","yScoresArray");
  d->RCalc->GetArray("loadingsArray","loadingsArray");
  d->RCalc->GetArray("loadingWeightsArray","loadingWeightsArray");
  d->RCalc->GetArray("yLoadingsArray","yLoadingsArray");
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
  "yScoresArray <- t(PLSresult$Yscores)\n"
  "loadingsArray <- PLSresult$loadings[,]\n"
  "loadingWeightsArray <- PLSresult$loading.weights[,]\n"
  "yLoadingsArray <- PLSresult$Yloadings[,]\n"
  ).arg(algorithmString).toLatin1());
  d->RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());

  // Get R output and check for errors "thrown" by R script
  if(!outputArrayData || outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() > 1)
    {
    qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
    return false;
    }

  // Get analyte names with row labels
  vtkNew<vtkStringArray> analyteNames;
  voUtils::addCounterLabels(extendedTable->GetRowMetaDataOfInterestAsString(),
                            analyteNames.GetPointer(), false);

  // Get experiment names with column labels
  vtkNew<vtkStringArray> experimentNames;
  voUtils::addCounterLabels(extendedTable->GetColumnMetaDataOfInterestAsString(),
                            experimentNames.GetPointer(), true);

  // ------------------------------------------------
  // Extract table for scores and Y-scores
  // No need to transpose scoresArray, it was done within the R code
  vtkNew<vtkTable> scoresTable;
  vtkNew<vtkTable> yScoresTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("scoresArray"), scoresTable.GetPointer());
    voUtils::arrayToTable(outputArrayData->GetArrayByName("yScoresArray"), yScoresTable.GetPointer());

    // Add column labels (experiment names)
    voUtils::setTableColumnNames(scoresTable.GetPointer(), experimentNames.GetPointer());
    voUtils::setTableColumnNames(yScoresTable.GetPointer(), experimentNames.GetPointer());

    // Add row labels (components)
    vtkNew<vtkStringArray> headerArr;
    for (vtkIdType r = 0;r < scoresTable->GetNumberOfRows(); ++r)
      {
      headerArr->InsertNextValue(QString("Comp %1").arg(r + 1).toLatin1());
      }
    voUtils::insertColumnIntoTable(scoresTable.GetPointer(), 0, headerArr.GetPointer());
    voUtils::insertColumnIntoTable(yScoresTable.GetPointer(), 0, headerArr.GetPointer());
    }
  this->setOutput("scores", new voTableDataObject("scores", scoresTable.GetPointer()));
  this->setOutput("yScores", new voTableDataObject("yScores", yScoresTable.GetPointer()));

  // ------------------------------------------------
  // Extract table for loadings and loading weights
  vtkNew<vtkTable> loadingsTable;
  vtkNew<vtkTable> loadingWeightsTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("loadingsArray"), loadingsTable.GetPointer());
    voUtils::arrayToTable(outputArrayData->GetArrayByName("loadingWeightsArray"), loadingWeightsTable.GetPointer());

    // Add column labels (components)
    for(vtkIdType c = 0; c < loadingsTable->GetNumberOfColumns(); ++c)
      {
      QByteArray colName = QString("Comp %1").arg(c + 1).toLatin1();
      loadingsTable->GetColumn(c)->SetName(colName);
      loadingWeightsTable->GetColumn(c)->SetName(colName);
      }

    // Add row labels (predictor analytes)
    vtkNew<vtkStringArray> headerArr;
    foreach(int r, predictorRangeList)
      {
      headerArr->InsertNextValue(analyteNames->GetValue(r));
      }
    voUtils::insertColumnIntoTable(loadingsTable.GetPointer(), 0, headerArr.GetPointer());
    voUtils::insertColumnIntoTable(loadingWeightsTable.GetPointer(), 0, headerArr.GetPointer());
    }
  this->setOutput("loadings", new voTableDataObject("loadings", loadingsTable.GetPointer()));
  vtkNew<vtkTable> loadingsTableTransposed;
  voUtils::transposeTable(loadingsTable.GetPointer(), loadingsTableTransposed.GetPointer(), voUtils::Headers);
  this->setOutput("loadings_transposed", new voTableDataObject("loadings_transposed", loadingsTableTransposed.GetPointer()));

  this->setOutput("loadingWeights", new voTableDataObject("loadingWeights", loadingWeightsTable.GetPointer()));
  vtkNew<vtkTable> loadingWeightsTableTransposed;
  voUtils::transposeTable(loadingWeightsTable.GetPointer(), loadingWeightsTableTransposed.GetPointer(), voUtils::Headers);
  this->setOutput("loadingWeights_transposed", new voTableDataObject("loadingWeights_transposed", loadingWeightsTableTransposed.GetPointer()));

  // ------------------------------------------------
  // Extract table for Y-loadings
  vtkNew<vtkTable> yLoadingsTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("yLoadingsArray"), yLoadingsTable.GetPointer());
    if(responseRangeList.size() == 1)
      {
      // arrayToTable is unable to determine the proper orientation of a 1-by-n array
      voUtils::transposeTable(yLoadingsTable.GetPointer());
      }

    // Add column labels (components)
    for(vtkIdType c = 0; c < yLoadingsTable->GetNumberOfColumns(); ++c)
      {
      yLoadingsTable->GetColumn(c)->SetName(QString("Comp %1").arg(c + 1).toLatin1());
      }

    // Add row labels (response analytes)
    vtkNew<vtkStringArray> headerArr;
    foreach(int r, responseRangeList)
      {
      headerArr->InsertNextValue(analyteNames->GetValue(r));
      }
    voUtils::insertColumnIntoTable(yLoadingsTable.GetPointer(), 0, headerArr.GetPointer());
    }
  this->setOutput("yLoadings", new voTableDataObject("yLoadings", yLoadingsTable.GetPointer()));
  vtkNew<vtkTable> yLoadingsTableTransposed;
  voUtils::transposeTable(yLoadingsTable.GetPointer(), yLoadingsTableTransposed.GetPointer(), voUtils::Headers);
  this->setOutput("yLoadings_transposed", new voTableDataObject("yLoadings_transposed", yLoadingsTableTransposed.GetPointer()));

  return true;
}
