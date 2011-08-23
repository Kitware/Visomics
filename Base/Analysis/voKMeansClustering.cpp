
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voKMeansClustering.h"
#include "voTableDataObject.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayToTable.h>
#include <vtkDataSetAttributes.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkRCalculatorFilter.h>
#include <vtkIntArray.h>

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
void voKMeansClustering::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voKMeansClustering::setOutputInformation()
{
  this->addOutputType("cluster", "vtkTable",
                      "", "",
                      "voTableView", "cluster");
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

  this->addParameterGroup("KMeans parameters", kmeans_parameters);
}

// --------------------------------------------------------------------------
bool voKMeansClustering::execute()
{
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qWarning() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::Take(extendedTable->GetDataWithRowHeader());

  // Parameters
  int kmeans_centers = this->integerParameter("centers");
  int kmeans_iter_max = this->integerParameter("iter.max");
  QString kmeans_algorithm = this->enumParameter("algorithm");

  vtkSmartPointer< vtkTableToArray > tableToArray = vtkSmartPointer< vtkTableToArray>::New();
  tableToArray->SetInput(table);

  vtkSmartPointer< vtkStringArray > names = vtkSmartPointer< vtkStringArray >::New();

  names->SetName("Samples");
  for (int ctr = 1; ctr < table->GetNumberOfColumns(); ctr++)
    {
    tableToArray->AddColumn(table->GetColumnName(ctr));
    names->InsertNextValue(table->GetColumnName(ctr));
    }
  tableToArray->Update();

  vtkSmartPointer< vtkRCalculatorFilter > calc = vtkSmartPointer< vtkRCalculatorFilter>::New();
  calc->SetRoutput(0);
  calc->SetInput(tableToArray->GetOutput());
  calc->PutArray("0", "metabData");
  calc->GetArray("kmCenters", "kmCenters");
  calc->GetArray("kmCluster", "kmCluster");
  calc->GetArray("kmWithinss", "kmWithinss");
  calc->GetArray("kmSize", "kmSize");
  calc->SetRscript(QString(
                     "metabDatat <- t(metabData)\n"
                     "km<-kmeans(metabDatat, %1, iter.max = %2, algorithm = \"%3\")\n"
                     "kmCenters<-km$centers \n"
                     "kmCluster<-km$cluster\n"
                     "kmWithinss<-km$withinss\n"
                     "kmSize<-km$size\n"
                     "kmCenters\n"
                     "kmCluster\n"
                     "kmWithinss\n"
                     "kmSize"
                     ).arg(kmeans_centers).arg(kmeans_iter_max).arg(kmeans_algorithm).toLatin1());

  calc->Update();

  vtkArrayData *temp = vtkArrayData::SafeDownCast(calc->GetOutput());
  if (!temp)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
    }

  vtkSmartPointer< vtkArrayData > kmReturn = vtkSmartPointer< vtkArrayData>::New();
  kmReturn->DeepCopy(temp);
/*
  vtkSmartPointer< vtkArrayData > kmCentersData = vtkSmartPointer< vtkArrayData>::New();
  kmCentersData->AddArray(kmReturn->GetArrayByName("kmCenters"));

  vtkSmartPointer< vtkArrayToTable > kmCenters = vtkSmartPointer< vtkArrayToTable>::New();
  kmCenters->SetInputConnection(kmCentersData->GetProducerPort());
  kmCenters->Update();
*/
  vtkSmartPointer< vtkArrayData > kmClusterData = vtkSmartPointer< vtkArrayData>::New();
  kmClusterData->AddArray(kmReturn->GetArrayByName("kmCluster"));

  vtkSmartPointer< vtkArrayToTable > kmCluster= vtkSmartPointer< vtkArrayToTable>::New();
  kmCluster->SetInputConnection(kmClusterData->GetProducerPort());
  kmCluster->Update();
/*
  vtkSmartPointer< vtkArrayData > kmWithinssData= vtkSmartPointer< vtkArrayData >::New();
  kmWithinssData->AddArray(kmReturn->GetArrayByName("kmWithinss"));

  vtkSmartPointer< vtkArrayToTable > kmWithinss= vtkSmartPointer< vtkArrayToTable >::New();
  kmWithinss->SetInputConnection(kmWithinssData->GetProducerPort());
  kmWithinss->Update();

  vtkSmartPointer< vtkArrayData > kmSizeData= vtkSmartPointer< vtkArrayData >::New();
  kmSizeData->AddArray(kmReturn->GetArrayByName("kmSize"));

  vtkSmartPointer< vtkArrayToTable > kmSize= vtkSmartPointer< vtkArrayToTable >::New();
  kmSize->SetInputConnection(kmSizeData->GetProducerPort());
  kmSize->Update();
*/

  // Create a table with sample name and cluster number
  // NOTE: This transpose is necessary because the one-dimensional vtkArray returned by R is
  //       oriented vertically. Attempting to transpose the array within R causes it to become
  //       a two-dimensional vtkArray when returned.
  //       To keep the orientation of experiments consistant between input and output, a
  //       transpose and manually building the table is necessary. If a vertially-oriented
  //       table is acceptable, simply output kmCluster->GetOutput()
  vtkSmartPointer< vtkTable > clusterTable = vtkSmartPointer< vtkTable >::New();

  vtkSmartPointer<vtkStringArray> headerCol = vtkSmartPointer<vtkStringArray>::New();
  headerCol->InsertNextValue(QObject::tr("Cluster number").toLatin1());
  clusterTable->AddColumn(headerCol);
  for(unsigned int i = 0; i < kmClusterData->GetArray(0)->GetSize(); ++i)
    {
    vtkSmartPointer<vtkIntArray> newCol = vtkSmartPointer<vtkIntArray>::New();
    newCol->SetName(table->GetColumnName(i+1)); // "table" contains a name column that must be offset from
    newCol->InsertNextValue(kmClusterData->GetArray(0)->GetVariantValue(i).ToInt());
    clusterTable->AddColumn(newCol);
    }

  this->setOutput("cluster", new voTableDataObject("cluster", clusterTable));

  return true;
}
