
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voApplication.h"
#include "voKMeansClustering.h"
#include "voTableDataObject.h"

// VTK includes
#include <vtkAdjacencyMatrixToEdgeTable.h>
#include <vtkAlgorithm.h>
#include <vtkArrayToTable.h>
#include <vtkDataSetAttributes.h>
#include <vtkDescriptiveStatistics.h>
#include <vtkDoubleArray.h>
#include <vtkGraph.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPCAStatistics.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkTableToGraph.h>
#include <vtkRCalculatorFilter.h>
#include <vtkDenseArray.h>
#include <vtkTree.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkIntArray.h>


// --------------------------------------------------------------------------
// voKMeansClustering methods

// --------------------------------------------------------------------------
voKMeansClustering::voKMeansClustering():
    Superclass()
{
  Q_D(voKMeansClustering);

}

// --------------------------------------------------------------------------
voKMeansClustering::~voKMeansClustering()
{
}

// --------------------------------------------------------------------------
void voKMeansClustering::setInputInformation()
{
  this->addInputType("input", "vtkTable");
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
  kmeans_parameters << this->addIntegerParameter("centers", tr("Number of clusters"), 2, 10, 4);

  // KMeans / MaxIter
  kmeans_parameters << this->addIntegerParameter("iter.max", tr("Max. iteration"), 5, 50, 10);

  // KMeans / Algorithm
  QStringList kmeans_algorithms;
  kmeans_algorithms << "Hartigan-Wong" << "Lloyd" << "Forgy" << "MacQueen";
  kmeans_parameters << this->addEnumParameter("algorithm", "Algorithm", kmeans_algorithms);

  this->addParameterGroup("KMeans parameters", kmeans_parameters);
}

// --------------------------------------------------------------------------
bool voKMeansClustering::execute()
{
  Q_D(voKMeansClustering);

  vtkTable* table =  vtkTable::SafeDownCast(this->input()->data());
  if (!table)
    {
    qWarning() << "Input is Null";
    return false;
    }

  // Parameters
  int kmeans_centers = this->integerParameter("centers");
  int kmeans_iter_max = this->integerParameter("iter.max");
  QString kmeans_algorithm = this->enumParameter("algorithm");

  // Transpose table
  vtkSmartPointer<vtkTable> transpose = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> header = vtkSmartPointer<vtkStringArray>::New();
  header->SetName("header");
  header->SetNumberOfTuples(table->GetNumberOfColumns()-1);
  for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
    {
    header->SetValue(c-1, table->GetColumnName(c));
    }
  transpose->AddColumn(header);
  for (vtkIdType r = 0; r < table->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkStringArray> newcol = vtkSmartPointer<vtkStringArray>::New();
    newcol->SetName(table->GetValue(r, 0).ToString().c_str());
    newcol->SetNumberOfTuples(table->GetNumberOfColumns() - 1);
    for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
      {
      newcol->SetValue(c-1, table->GetValue(r, c).ToString());
      }
    transpose->AddColumn(newcol);
    }

 
  vtkSmartPointer< vtkTableToArray > tableToArray = vtkSmartPointer< vtkTableToArray>::New();
  tableToArray->SetInput(transpose);

  vtkSmartPointer< vtkStringArray > names = vtkSmartPointer< vtkStringArray >::New();

  int start = 1;
  int end = transpose->GetNumberOfColumns();

  std::cout << "Total number of columns:\t" << end << std::endl;

  names->SetName("Samples");
  for (int ctr=start; ctr<end; ctr++)
    {
    tableToArray->AddColumn(transpose->GetColumnName(ctr));
    names->InsertNextValue(transpose->GetColumnName(ctr));
    }
  tableToArray->Update();

  //Print ouf the array data for debugging purposes
  vtkSmartPointer< vtkArrayData > arrayData = vtkSmartPointer< vtkArrayData>::New();
  arrayData = tableToArray->GetOutput();

  std::cout << "Array data sent to R \n" << std::endl;

  vtkIdType numberOfArrays = arrayData->GetNumberOfArrays();

  for( vtkIdType i=0; i < numberOfArrays; i++)
    {
    vtkDenseArray<double> *array = vtkDenseArray<double>::SafeDownCast(arrayData->GetArray(i));
    std::cout << "Dimensions:" << array->GetDimensions() << std::endl;
    const vtkArrayExtents extents = array->GetExtents();
    for(vtkIdType i = extents[0].GetBegin(); i != extents[0].GetEnd(); ++i)
      {   
      for(vtkIdType j = extents[1].GetBegin(); j != extents[1].GetEnd(); ++j)
        {
        std::cout << array->GetValue(vtkArrayCoordinates(i, j)) << "\t";
        }
        std::cout << std::endl;
      }   
    }

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

  calc->GetOutput()->Print(std::cout);
  vtkArrayData *temp = vtkArrayData::SafeDownCast(calc->GetOutput());
  if (!temp)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
    }

  vtkSmartPointer< vtkArrayData > kmReturn = vtkSmartPointer< vtkArrayData>::New();
  kmReturn->DeepCopy(temp);

  vtkSmartPointer< vtkArrayData > kmCentersData = vtkSmartPointer< vtkArrayData>::New();
  kmCentersData->AddArray(kmReturn->GetArrayByName("kmCenters"));

  vtkSmartPointer< vtkArrayToTable > kmCenters = vtkSmartPointer< vtkArrayToTable>::New();
  kmCenters->SetInputConnection(kmCentersData->GetProducerPort());
  kmCenters->Update();

  vtkSmartPointer< vtkArrayData > kmClusterData = vtkSmartPointer< vtkArrayData>::New();
  kmClusterData->AddArray(kmReturn->GetArrayByName("kmCluster"));

  vtkSmartPointer< vtkArrayToTable > kmCluster= vtkSmartPointer< vtkArrayToTable>::New();
  kmCluster->SetInputConnection(kmClusterData->GetProducerPort());
  kmCluster->Update();

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

  // Create a table with sample name and cluster number
  //
  vtkSmartPointer< vtkTable > clusterTable = vtkSmartPointer< vtkTable >::New();
  vtkSmartPointer< vtkIntArray > clusterNumber = vtkSmartPointer< vtkIntArray >::New();

  clusterNumber->SetName("Cluster number");

  const vtkArrayExtents extents = kmClusterData->GetArray(0)->GetExtents();
  vtkDenseArray<double> * array = vtkDenseArray<double>::SafeDownCast(kmClusterData->GetArray(0));
  for(int i = 0; i != extents[0].GetEnd(); ++i)
    {
    //std::cout << names->GetValue(i) << "\t"  << array->GetValue(i) << std::endl;
    clusterNumber->InsertNextValue( array->GetValue(i));
    }
  clusterTable->AddColumn(names);
  clusterTable->AddColumn(clusterNumber);
  clusterTable->Dump();
 
  this->setOutput("cluster", new voTableDataObject("cluster", clusterTable));

  return true;
}
