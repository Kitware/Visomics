
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voDataObject.h"
#include "voHierarchicalClustering.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkDataSetAttributes.h>
#include <vtkArrayData.h>
#include <vtkDenseArray.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTree.h>
#include <vtkAdjacentVertexIterator.h>
#include <vtkTreeDFSIterator.h>
#include <vtkTreeBFSIterator.h>

// --------------------------------------------------------------------------
// voHierarchicalClustering methods

// --------------------------------------------------------------------------
voHierarchicalClustering::voHierarchicalClustering():
    Superclass()
{
}

// --------------------------------------------------------------------------
voHierarchicalClustering::~voHierarchicalClustering()
{
}

// --------------------------------------------------------------------------
void voHierarchicalClustering::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voHierarchicalClustering::setOutputInformation()
{
  this->addOutputType("clusterTree", "vtkTree",
                      "voTreeGraphView", "Cluster Tree");

  this->addOutputType("clusterHeatMap", "vtkTable",
                      "voHeatMapView", "Clustered Data (HeatMap)",
                      "voTableView", "Clustered Data (Table)");

  this->addOutputType("clusterTree-dynview", "vtkTable",
                      "voHierarchicalClusteringDynView", "Interactive Hierarchical Clustering Plot (prototype)",
                      "", "");
}

// --------------------------------------------------------------------------
void voHierarchicalClustering::setParameterInformation()
{
  QList<QtProperty*> hclust_parameters;

  // HClust / Method
  QStringList hclust_methods;
  hclust_methods << "complete" << "average" << "mcquitty" << "median" << "centroid";
  hclust_parameters << this->addEnumParameter("method", tr("Method"), hclust_methods, "average");

  this->addParameterGroup("HClust parameters", hclust_parameters);
}

// --------------------------------------------------------------------------
bool voHierarchicalClustering::execute()
{
  // Import data table
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qWarning() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> inputDataTable = extendedTable->GetData();

  vtkSmartPointer<vtkStringArray> columnNames = extendedTable->GetColumnMetaDataOfInterestAsString();

  // Parameters
  QString hclust_method = this->enumParameter("method");

  // R input
  vtkSmartPointer<vtkArray> RInputArray;
  voUtils::tableToArray(inputDataTable.GetPointer(), RInputArray);

  vtkNew<vtkArrayData> RInputArrayData;
  RInputArrayData->AddArray(RInputArray.GetPointer());

  // Run R
  vtkNew <vtkRCalculatorFilter> RCalc;
  RCalc->SetRoutput(0);
  RCalc->SetInputConnection(RInputArrayData->GetProducerPort());
  RCalc->PutArray("0", "metabData");
  RCalc->GetArray("height","height");
//  RCalc->GetArray("order","order");
  RCalc->GetArray("merge","merge");
  RCalc->SetRscript(QString(
                     "dEuc<-dist(t(metabData))\n"
                     "cluster<-hclust(dEuc,method=\"%1\")\n"
                     "height<-cluster$height\n"
//                     "order<-cluster$order\n"
                     "merge<-cluster$merge\n"
                     ).arg(hclust_method).toLatin1());
  RCalc->Update();

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

  // Get R output
  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(RCalc->GetOutput());

  vtkSmartPointer< vtkDenseArray<double> > heightArray;
  heightArray = vtkDenseArray<double>::SafeDownCast(outputArrayData->GetArrayByName("height"));

  vtkSmartPointer< vtkDenseArray<double> > mergeArray;
  mergeArray = vtkDenseArray<double>::SafeDownCast(outputArrayData->GetArrayByName("merge"));

  // Analysis outputs
  vtkNew<vtkMutableDirectedGraph> graph;

  vtkNew<vtkStringArray> clusterLabelArray; // Array to label the vertices
  clusterLabelArray->SetName("id");

  vtkNew<vtkDoubleArray> distanceArray; // Array to store the vertices height
  distanceArray->SetName("Height");

  // Build graph
  vtkNew<vtkIntArray> clusterMap; // [clusterIndex] -> clusterVertexID
  clusterMap->SetNumberOfValues(mergeArray->GetExtent(0).GetSize());

  for(int clusterIndex = 0; clusterIndex < mergeArray->GetExtent(0).GetSize(); ++clusterIndex)
    {
    // R cluster index starts from 1; when firstCluster/secondCluster is used, its value must be taken as: abs(cluster) - 1
    int firstCluster  =  mergeArray->GetValue(clusterIndex, 0);
    int secondCluster =  mergeArray->GetValue(clusterIndex, 1);

    double heightParent =  heightArray->GetValue(clusterIndex);
    double heightChildren = heightParent - 0.1;// arbitrary

    vtkIdType parentID = graph->AddVertex();
    clusterLabelArray->InsertNextValue ("");
    distanceArray->InsertNextValue(heightParent);

    clusterMap->SetValue(clusterIndex, parentID);

    vtkIdType child1ID;
    if ( firstCluster < 0 )
      {
      child1ID = graph->AddVertex();
      clusterLabelArray->InsertNextValue(columnNames->GetValue(abs(firstCluster) - 1));
      distanceArray->InsertNextValue(heightChildren);
      }
    else // ( firstCluster > 0 )
      {
      child1ID = clusterMap->GetValue(firstCluster - 1);
      }

    vtkIdType child2ID;
    if (secondCluster < 0)
      {
      child2ID = graph->AddVertex();
      clusterLabelArray->InsertNextValue(columnNames->GetValue(abs(secondCluster) - 1));
      distanceArray->InsertNextValue(heightChildren);
      }
    else // ( secondCluster > 0 )
      {
      child2ID = clusterMap->GetValue(secondCluster - 1);
      }

    graph->AddEdge( parentID, child1ID );
    graph->AddEdge( parentID, child2ID );
    }

  vtkNew<vtkTree> tree;
  tree->ShallowCopy(graph.GetPointer());

  //Add vertex attributes
  tree->GetVertexData()->AddArray(clusterLabelArray.GetPointer());
  tree->GetVertexData()->AddArray(distanceArray.GetPointer());

  this->setOutput("clusterTree", new voDataObject("clusterTree", tree.GetPointer()));

  // Generate a list of data in reverse BFS (bottom-up) order;
  // We may want a different order later, if we overlay a cluster tree on the tabular data
  // For now, this puts data with the strongest clustering on the left
  QStringList reverseBFSLabels;
    {
    vtkNew<vtkTreeBFSIterator> bfs;
    bfs->SetTree(tree.GetPointer());
    int prevTreeLevel = 0;
    QStringList levelLabels;
    vtkStringArray* treeLabels = vtkStringArray::SafeDownCast(tree->GetVertexData()->GetAbstractArray("id"));
    while (bfs->HasNext())
      {
      vtkIdType curVertex = bfs->Next();
      vtkIdType curLevel = tree->GetLevel(curVertex);
      if (prevTreeLevel != curLevel)
        {
        reverseBFSLabels = levelLabels + reverseBFSLabels; // Prepend level list to master list
        levelLabels.clear();
        prevTreeLevel = curLevel;
        }
      if (treeLabels->GetValue(curVertex) != "")
        {
        levelLabels << QString(treeLabels->GetValue(curVertex));
        }
      }
    reverseBFSLabels = levelLabels + reverseBFSLabels;
    }
  this->setOutput("clusterTree-dynview", new voTableDataObject("clusterTree-dynview", tree.GetPointer()));

  // Generate table for heatmap
  vtkNew<vtkTable> clusterTable;
  clusterTable->AddColumn(extendedTable->GetRowMetaDataOfInterestAsString());
  foreach(QString colLabel, reverseBFSLabels)
    {
    clusterTable->AddColumn(inputDataTable->GetColumnByName(colLabel.toLatin1()));
    }
  this->setOutput("clusterHeatMap", new voTableDataObject("clusterHeatMap", clusterTable.GetPointer()));

  return true;
}
