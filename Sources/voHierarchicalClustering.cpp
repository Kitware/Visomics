
// Qt includes
#include <QDebug>

// Visomics includes
#include "voApplication.h"
#include "voHierarchicalClustering.h"
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
// voHierarchicalClustering methods

// --------------------------------------------------------------------------
voHierarchicalClustering::voHierarchicalClustering():
    Superclass()
{
  Q_D(voHierarchicalClustering);

}

// --------------------------------------------------------------------------
voHierarchicalClustering::~voHierarchicalClustering()
{
}

// --------------------------------------------------------------------------
void voHierarchicalClustering::setInputInformation()
{
  this->addInputType("input", "vtkTable");
}

// --------------------------------------------------------------------------
void voHierarchicalClustering::setOutputInformation()
{
  this->addOutputType("clusterTree", "vtkTree",
                      "voTreeGraphView", "clusterTree");
}

// --------------------------------------------------------------------------
bool voHierarchicalClustering::execute()
{
  Q_D(voHierarchicalClustering);

  vtkTable* table =  vtkTable::SafeDownCast(this->input()->data());
  if (!table)
    {
    qWarning() << "Input is Null";
    return false;
    }

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

  /* 
   * hclust class in R has the following attributes 
   *
   * labels: labels for each of the objects being clustered.
   *
   * merge: an n-1 by 2 matrix.  Row i of merge describes the
   *       merging of clusters at step i of the clustering.  If an
   *       element j in the row is negative, then observation -j was
   *       merged at this stage.  If j is positive then the merge was
   *      with the cluster formed at the (earlier) stage j of the
   *      algorithm.  Thus negative entries in merge indicate
   *       agglomerations of singletons, and positive entries indicate
   *       agglomerations of non-singletons. 
   *
   * 
   * height: a set of n-1 non-decreasing real values.  The clustering
   *       _height_: that is, the value of the criterion associated with
   *       the clustering method for the particular agglomeration.
   *  	
   * order: a vector giving the permutation of the original observations
   *       suitable for plotting, in the sense that a cluster plot using
   *       this ordering and matrix merge will not have crossings
   *       of the branches.
   * 	
   *
   * labels : labels for each of the objects being clustered.
   *
   */
  calc->PutArray("0", "metabData");
  calc->GetArray("height","height");
  calc->GetArray("order","order");
  calc->GetArray("merge","merge");
  calc->SetRscript("dEuc<-dist(t(metabData))\n\
          cluster<-hclust(dEuc,method=\"average\")\n\
          height<-as.numeric(cluster$height)\n\
          order<-as.numeric(cluster$order)\n\
          merge<-as.numeric(cluster$merge)\n"); 

  calc->Update();

  //calc->GetOutput()->Print(std::cout);
  vtkArrayData *temp = vtkArrayData::SafeDownCast(calc->GetOutput());
  if (!temp)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
    }

  vtkSmartPointer<vtkArrayData> clustReturn = vtkSmartPointer<vtkArrayData>::New();
  clustReturn->DeepCopy(temp);

  vtkSmartPointer<vtkArrayData> heightData = vtkSmartPointer<vtkArrayData>::New();
  heightData->AddArray(clustReturn->GetArrayByName("height"));

  std::cout << "Cluster tree heights information..." << std::endl;
  vtkDenseArray<double> *heigtArray = vtkDenseArray<double>::SafeDownCast(heightData->GetArray(0));
  const vtkArrayExtents heightExtent = heigtArray->GetExtents();
  for(int i = 0; i != heightExtent[0].GetEnd(); ++i)
    {   
    std::cout << "\t" << heigtArray->GetValue(vtkArrayCoordinates(i)) << std::endl;
    }

  vtkSmartPointer<vtkArrayData> orderData = vtkSmartPointer<vtkArrayData>::New();
  orderData->AddArray(clustReturn->GetArrayByName("order"));

  vtkSmartPointer<vtkArrayData> mergeData = vtkSmartPointer<vtkArrayData>::New();
  mergeData->AddArray(clustReturn->GetArrayByName("merge"));

  vtkDenseArray<double> *mergeArray = vtkDenseArray<double>::SafeDownCast(mergeData->GetArray(0));

  const vtkArrayExtents matrixExtent = mergeArray->GetExtents();

  /*
  for(int i = 0; i != matrixExtent[0]; ++i)
    {   
    std::cout << "\t" << mergeArray->GetValue(vtkArrayCoordinates(i)) << std::endl;
    }
  */

  /* Since, VTK-R adapter unrolls the merge matrix, start first reconstructing the merge matrix */
  std::cout << "Reconstructing the merge matrix" << std::endl;

  //Start constructing the graph too
  vtkSmartPointer<vtkMutableDirectedGraph> builder = vtkSmartPointer<vtkMutableDirectedGraph>::New();

  //generate array to label the vertices
  vtkSmartPointer<vtkStringArray> clusterLabel = vtkSmartPointer<vtkStringArray>::New();
  clusterLabel->SetName("id");

  //generate array to store the vertices height
  vtkSmartPointer<vtkDoubleArray> distanceArray = vtkSmartPointer<vtkDoubleArray>::New();
  distanceArray->SetName("Height");

 
  /* Each time we create a parent "id", store the corresponding vertex id */
     
  vtkstd::map<int,int> clusterMap;

  int clusterIndex=0;


  for(int i = 0; i != heightExtent[0].GetEnd(); ++i)
    {
    int firstClusterIndex;
    int secondClusterIndex;
  
    /* The following is needed to find the corresponding indices in the matrix */ 
    firstClusterIndex = i;
    secondClusterIndex = firstClusterIndex + heightExtent[0].GetEnd();
   
    int firstCluster  =  mergeArray->GetValue(vtkArrayCoordinates(firstClusterIndex));
    int secondCluster =  mergeArray->GetValue(vtkArrayCoordinates(secondClusterIndex));
 
    std::cout << "\t" << firstCluster<< "\t" << secondCluster << std::endl;

    /** Three scenario:
    *  if both  values are negative, create two new childs and a new vertex in the tree
    *  if either one is positive,  create a new child and join it with already existing vertex
    *  if both positive, join two already existing vertices in the tree
    */

    std::cout << "\t\t " << transpose->GetColumnName( abs(firstCluster) )  << "\t" << table->GetColumnName( abs(secondCluster)) << std::endl;
    if( firstCluster < 0 && secondCluster < 0 )
      {
      std::cout << "\t\t\t" << "Two new clusters with the same parent" << std::endl;
      vtkIdType parent = builder->AddVertex();
      clusterLabel->InsertNextValue( "" );
      std::cout << "\t\t\t\t\t Add a parent vertex with id: " << parent << std::endl;
      clusterMap[clusterIndex] = parent;
      std::cout << "\t\t\t\t\t\t Added it to the map: " << clusterIndex << "\t" << clusterMap[clusterIndex] << std::endl;
      clusterIndex++;

      double heightParent =  heigtArray->GetValue(vtkArrayCoordinates(i));
      double heightChildrean = heightParent - 0.02; // arbitrary
      distanceArray->InsertNextValue(heightParent);
      std::cout << "\t\t\t\t\t\t\t\tVertices Height:\t" << heightParent << "\t" << heightChildrean << std::endl;
      
      
      vtkIdType child1 = builder->AddVertex();
      clusterLabel->InsertNextValue( transpose->GetColumnName( abs(firstCluster)) );
      distanceArray->InsertNextValue(heightChildrean);
      std::cout << "\t\t\t\t\t Add a new vertex with id: " << child1 << std::endl;

      vtkIdType child2 = builder->AddVertex();
      clusterLabel->InsertNextValue( transpose->GetColumnName( abs(secondCluster)) );
      distanceArray->InsertNextValue(heightChildrean);
      std::cout << "\t\t\t\t\t Add a new vertex with id: " << child2 << std::endl;

      builder->AddEdge( parent, child1);
      builder->AddEdge( parent, child2);
      std::cout << "\t\t\t\t\t" << "Connect the two vertices to the parent: " << child1  << "\t" << child2 << "\t\t" << parent << std::endl;

      }
    else if( firstCluster > 0 && secondCluster > 0 )
      {
      std::cout << "\t\t\t" << "Two clusters joined to the same parent" << std::endl;
      vtkIdType parent = builder->AddVertex();
      clusterLabel->InsertNextValue ( "");
      clusterMap[clusterIndex] = parent;
      std::cout << "\t\t\t\t\t\t Added it to the map: " << clusterIndex << "\t" << clusterMap[clusterIndex] << std::endl;
      clusterIndex++;
      std::cout << "\t\t\t\t\t Add a parent vertex with id: " << parent << std::endl;

      double heightParent =  heigtArray->GetValue(vtkArrayCoordinates(i));
      double heightChildrean = heightParent - 0.1; // arbitrary
      distanceArray->InsertNextValue(heightParent);
      std::cout << "\t\t\t\t\t\t\t\tVertices Height:\t" << heightParent << "\t" << heightChildrean << std::endl;
     
     
      int clusterNumber1 = clusterMap[firstCluster - 1];
      int clusterNumber2 = clusterMap[secondCluster - 1]; 
  
      builder->AddEdge( parent, clusterNumber1 );
      builder->AddEdge( parent, clusterNumber2 );
      std::cout << "\t\t\t\t\t" << "Connect the two vertices to the parent: " << clusterNumber1  << "\t" << clusterNumber2 << "\t\t" << parent << std::endl;
      }
    else
      {
      std::cout << "\t\t\t" << "A new and old cluster joined to to the same parent" << std::endl;

      if ( firstCluster < 0 )
        {
        vtkIdType parent = builder->AddVertex();
        clusterLabel->InsertNextValue ( "");
        clusterMap[clusterIndex] = parent;
        std::cout << "\t\t\t\t\t\t Added it to the map: " << clusterIndex << "\t" << clusterMap[clusterIndex] << std::endl;
        clusterIndex++;
 
        std::cout << "\t\t\t\t\t Add a parent vertex with id: " << parent << std::endl;

        double heightParent =  heigtArray->GetValue(vtkArrayCoordinates(i));
        double heightChildrean = heightParent - 0.1;// arbitrary
        distanceArray->InsertNextValue(heightParent);
        std::cout << "\t\t\t\t\t\t\t\tVertices Height:\t" << heightParent << "\t" << heightChildrean << std::endl;

        
        vtkIdType child = builder->AddVertex(); 
        std::cout << "\t\t\t\t\t Add a new vertex with id: " << child << std::endl;
        clusterLabel->InsertNextValue( transpose->GetColumnName( abs(firstCluster)) );
        distanceArray->InsertNextValue(heightChildrean);

        int clusterNumber  = clusterMap[secondCluster - 1]; // R cluster index starts from 1
        std::cout << "\t\t\t\t" << "VTK ID number correspond to R cluster ID\t" << secondCluster-1 << "\t" << clusterNumber << std::endl;

        builder->AddEdge( parent, child );
        builder->AddEdge( parent, clusterNumber );
        std::cout << "\t\t\t\t\t" << "Connect the two vertices to the parent: " << child  << "\t" << clusterNumber << "\t\t" << parent << std::endl;
        }
      else
        {
        vtkIdType parent = builder->AddVertex();
        clusterLabel->InsertNextValue ( "");
        clusterMap[clusterIndex] = parent;
        std::cout << "\t\t\t\t\t\t Added it to the map: " << clusterIndex << "\t" << clusterMap[clusterIndex] << std::endl;
        clusterIndex++;
        std::cout << "\t\t\t\t\t Add a parent vertex with id: " << parent << std::endl;

        double heightParent =  heigtArray->GetValue(vtkArrayCoordinates(i));
        double heightChildrean = heightParent-0.1; // arbitrary
        distanceArray->InsertNextValue(heightParent);
        std::cout << "\t\t\t\t\t\t\t\tVertices Height:\t" << heightParent << "\t" << heightChildrean << std::endl;
        
        vtkIdType child = builder->AddVertex(); 
        std::cout << "\t\t\t\t\t Add a new vertex with id: " << child << std::endl;
        clusterLabel->InsertNextValue( transpose->GetColumnName( abs(secondCluster)) );
        distanceArray->InsertNextValue(heightChildrean);
        

        int clusterNumber = clusterMap[firstCluster - 1]; // R cluster index start from 1
        builder->AddEdge( parent, child );
        builder->AddEdge( parent, firstCluster );
        std::cout << "\t\t\t\t\t" << "Connect the two vertices to the parent: " << child  << "\t" << clusterNumber << "\t\t" << parent << std::endl;
        }
      }
    }
  
  vtkSmartPointer<vtkTree> tree = vtkSmartPointer<vtkTree>::New();
  tree->ShallowCopy(builder);

  //Add vertex attributes
  tree->GetVertexData()->AddArray(clusterLabel);
  tree->GetVertexData()->AddArray(distanceArray);

  this->setOutput("clusterTree", new voDataObject("clusterTree", tree));

  return true;
}
