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
#include "voKMeansClustering.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voKMeansClustering methods

// --------------------------------------------------------------------------
voKMeansClustering::voKMeansClustering():
    Superclass()
{
  // Q_D(voKMeansClustering);
}

// --------------------------------------------------------------------------
voKMeansClustering::~voKMeansClustering()
{
}


// --------------------------------------------------------------------------
void voKMeansClustering::setOutputInformation()
{
  this->addOutputType("cluster", "vtkTable",
                      "voKMeansClusteringDynView", "Clusters (Dendogram)",
                      "voTableView", "Clusters (Table)");
}

// --------------------------------------------------------------------------
void voKMeansClustering::setParameterInformation()
{
  QList<QtProperty*> kmeans_parameters;

  // KMeans / Number of clusters (centers)
  kmeans_parameters << this->addIntegerParameter("centers", QObject::tr("Number of clusters"), 2, 10, 4);

  // KMeans / MaxIter
  kmeans_parameters << this->addIntegerParameter("iter.max", QObject::tr("Max. iteration"), 5, 50, 10);

  // KMeans / Algorithm
  QStringList kmeans_algorithms;
  kmeans_algorithms << "Hartigan-Wong" << "Lloyd" << "Forgy" << "MacQueen";
  kmeans_parameters << this->addEnumParameter("algorithm", "Algorithm", kmeans_algorithms);

   // KMeans / number of random start
  kmeans_parameters << this->addIntegerParameter("nstart", QObject::tr("Number of random start"), 1, 50, 10);

  this->addParameterGroup("KMeans parameters", kmeans_parameters);
}

// --------------------------------------------------------------------------
QString voKMeansClustering::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Number of clusters</b>:</dt>"
                 "<dd>The value of <i>k</i>: the number of clusters.</dd>"
                 "<dt><b>Max. iteration</b>:</dt>"
                 "<dd>A larger number of iterations will take longer, "
                 "but be more likely to converge on a better solution.</dd>"
                 "<dt><b>Algorithm</b>:</dt>"
                 "<dd>The specific algorithm for the k-means method.</dd>"
                 "<dt><b>Number of random start</b>:</dt>"
                 "<dd>Number of initially random cluster sets that the "
                 "algorithm will attempt to refine. "
                 "A larger number will take longer, "
                 "but be more likely to converge on a better solution.</dd>"
                 "</dl>");
}

namespace
{
// --------------------------------------------------------------------------
QList<int> collectColumnIds(vtkTable* table, int value)
{
  Q_ASSERT(table);
  QList<int> ids;
  for(vtkIdType cid = 1; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkIntArray * currentColumn = vtkIntArray::SafeDownCast(table->GetColumn(cid));
    int currentValue = currentColumn->GetValue(0);
    if (currentValue == value)
      {
      ids << cid;
      }
    }
  return ids;
}

// --------------------------------------------------------------------------
void swapClusterIds(vtkTable* table, const QList<int>& columndIds1, int value1,
                    const QList<int>& columnIds2, int value2)
{
  Q_ASSERT(table);
  Q_ASSERT(!columndIds1.isEmpty());
  Q_ASSERT(!columnIds2.isEmpty());
  foreach(int cid, columndIds1)
    {
    vtkIntArray::SafeDownCast(table->GetColumn(cid))->SetValue(0, value2);
    }
  foreach(int cid, columnIds2)
    {
    vtkIntArray::SafeDownCast(table->GetColumn(cid))->SetValue(0, value1);
    }
}

// --------------------------------------------------------------------------
void displayClustedIds(const char* description, vtkTable* table)
{
  Q_ASSERT(table);
  QList<int> columndIdsToDisplay;
  for(vtkIdType cid = 1; cid < table->GetNumberOfColumns(); ++cid)
    {
    columndIdsToDisplay << vtkIntArray::SafeDownCast(table->GetColumn(cid))->GetValue(0);
    }
  qDebug() << description << columndIdsToDisplay;
}

} // end of anonymous namespace

// --------------------------------------------------------------------------
bool voKMeansClustering::execute()
{
  // Parameters
  int kmeans_centers = this->integerParameter("centers");
  int kmeans_iter_max = this->integerParameter("iter.max");
  int kmeans_number_of_random_start = this->integerParameter("nstart");
  QString kmeans_algorithm = this->enumParameter("algorithm");

  // Import data table
  vtkSmartPointer<vtkExtendedTable> extendedTable = this->getInputTable();
  if (!extendedTable)
    {
    qCritical() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> inputDataTable = extendedTable->GetData();

  // Build ArrayData for input to R
  vtkNew<vtkArrayData> RInputArrayData;
    {
    vtkSmartPointer<vtkArray> RInputArray;
    voUtils::tableToArray(inputDataTable.GetPointer(), RInputArray);
    RInputArrayData->AddArray(RInputArray.GetPointer());
    }

  // Run R
  vtkNew<vtkRCalculatorFilter> RCalc;
  RCalc->SetRoutput(0);
  RCalc->SetInputData(RInputArrayData.GetPointer());
  RCalc->PutArray("0", "metabData");
  RCalc->GetArray("kmCenters", "kmCenters");
  RCalc->GetArray("kmCluster", "kmCluster");
  RCalc->GetArray("kmWithinss", "kmWithinss");
  RCalc->GetArray("kmSize", "kmSize");
  RCalc->SetRscript(QString(
                     "metabDatat <- t(metabData)\n"
                     "km<-kmeans(metabDatat, %1, iter.max = %2, nstart = %3, algorithm = \"%4\")\n"
                     "kmCenters<-km$centers \n"
                     "kmCluster<-km$cluster\n"
                     "kmWithinss<-km$withinss\n"
                     "kmSize<-km$size\n"
                     ).arg(kmeans_centers).arg(kmeans_iter_max).arg(kmeans_number_of_random_start).arg(kmeans_algorithm).toLatin1());
  RCalc->Update();

  // Get R output
  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(RCalc->GetOutput());
  if(!outputArrayData || !outputArrayData->GetArrayByName("kmCluster"))
    {
    qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
    return false;
    }

  // Get experiment names with column labels
  vtkNew<vtkStringArray> columnNames;
  voUtils::addCounterLabels(extendedTable->GetColumnMetaDataOfInterestAsString(),
                            columnNames.GetPointer(), true);

  // Get base cluster table
  vtkNew<vtkTable> clusterTable;
    {
    vtkSmartPointer<vtkArray> rawClusterArray = outputArrayData->GetArrayByName("kmCluster");

    vtkNew<vtkStringArray> tempHeaderCol;
    tempHeaderCol->InsertNextValue(QObject::tr("Cluster number").toLatin1());
    clusterTable->AddColumn(tempHeaderCol.GetPointer());

    for(unsigned int i = 0; i < rawClusterArray->GetSize(); ++i)
      {
      vtkNew<vtkIntArray> newCol;
      newCol->SetName(columnNames->GetValue(i));
      newCol->InsertNextValue(rawClusterArray->GetVariantValue(i).ToInt());
      clusterTable->AddColumn(newCol.GetPointer());
      }
    }

  // Renumber cluster table to be consistent
  // Since the cluster id associated with each columns is arbitrary,
  // let's make sure two successive execution of the analysis outputs
  // the same result by re-labelling the cluster id of each column from left to right.
  //displayClustedIds("initial", clusterTable.GetPointer());
  int futureClusterId = 1;
  QList<int> processedClusterIds;
  for(vtkIdType cid = 1; cid < clusterTable->GetNumberOfColumns(); ++cid)
    {
    vtkIntArray * currentColumn = vtkIntArray::SafeDownCast(clusterTable->GetColumn(cid));
    int currentClusterId = currentColumn->GetValue(0);
    if (processedClusterIds.contains(currentClusterId))
      {
      continue;
      }
    QList<int> rightColumnIds = collectColumnIds(clusterTable.GetPointer(), currentClusterId);
    QList<int> leftColumnIds = collectColumnIds(clusterTable.GetPointer(), futureClusterId);
    swapClusterIds(clusterTable.GetPointer(), rightColumnIds, currentClusterId, leftColumnIds, futureClusterId);
    processedClusterIds << futureClusterId;
    ++futureClusterId;
    }
  //displayClustedIds("final", clusterTable.GetPointer());

  voTableDataObject * dataObject = new voTableDataObject("cluster", clusterTable.GetPointer());
  dataObject->setProperty("kmeans_centers", kmeans_centers);
  this->setOutput("cluster", dataObject);

  return true;
}
