
// Qt includes
#include <QDebug>

// Visomics includes
#include "voApplication.h"
#include "voPCAStatistics.h"
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
//#include <vtkPCAStatistics.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkTableToGraph.h>

#include <vtkRCalculatorFilter.h>
#include <vtkDelimitedTextReader.h>
#include <vtkPlot.h>
#define VTK_ALLOCATE_MACRO(name,type) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New();


// --------------------------------------------------------------------------
class voPCAStatisticsPrivate
{
public:

  vtkSmartPointer<vtkDescriptiveStatistics> Descriptive;
  vtkSmartPointer<vtkRCalculatorFilter> PCA;
};

// --------------------------------------------------------------------------
// voPCAStatisticsPrivate methods

// --------------------------------------------------------------------------
// voPCAStatistics methods

// --------------------------------------------------------------------------
voPCAStatistics::voPCAStatistics():
    Superclass(), d_ptr(new voPCAStatisticsPrivate)
{
  Q_D(voPCAStatistics);
  
  d->PCA = vtkSmartPointer<vtkRCalculatorFilter>::New();
  /*d->PCA->SetLearnOption(true);
  d->PCA->SetDeriveOption(true);
  d->PCA->SetAssessOption(true);
  d->PCA->SetBasisSchemeByName("FixedBasisEnergy");
  d->PCA->SetFixedBasisEnergy(0.95);*/

  d->Descriptive = vtkSmartPointer<vtkDescriptiveStatistics>::New();
}

// --------------------------------------------------------------------------
voPCAStatistics::~voPCAStatistics()
{
}

// --------------------------------------------------------------------------
void voPCAStatistics::setInputInformation()
{
  this->addInputType("input", "vtkTable");
}

// --------------------------------------------------------------------------
void voPCAStatistics::setOutputInformation()
{
#if 0
  this->addOutputType("corr", "vtkTable",
                      "", "",
                      "voTableView", "Table (Correlation)");

  this->addOutputType("correlation graph", "vtkGraph",
                      "voCorrelationGraphView", "Correlation Graph");
  
  
  this->addOutputType("loading", "vtkTable",
                      "", "",
                      "voTableView", "Table (Percent Loading)");

  this->addOutputType("x", "vtkTable",
                      "", "",
                      "voTableView", "Table (Coordinates)");
#endif

  this->addOutputType("x", "vtkTable",
                      "voPCAProjectionPlot", "Projection Plot",
                      "voTableView", "Table (Projection Plot)");

  this->addOutputType("rot", "vtkTable",
                      "",  "",
                      "voTableView", "Table (Rotation)");

  this->addOutputType("sdev", "vtkTable",
                      "", "",
                      "voTableView", "Table (Std. Deviation)");
    
  this->addOutputType("loading", "vtkTable" ,
                      "voPCABarPlot", "Percent Loading Plot",
                      "voTableView", "Table (Percent Loading Plot)");
   
  this->addOutputType("sumloading", "vtkTable" ,
                      "voPCABarPlot", "Cumulative Percent Loading Plot",
                      "voTableView", "Table ( Cumulative Percent Loading Plot)");
}

// --------------------------------------------------------------------------
bool voPCAStatistics::execute()
{
  Q_D(voPCAStatistics);

  vtkTable* table =  vtkTable::SafeDownCast(this->input()->data());
  if (!table)
    {
    qWarning() << "Input is Null";
    return false;
    }

  /*// Add request to process all columns
  d->PCA->ResetRequests();
  d->PCA->ResetAllColumnStates();
  for (int i = 1; i < table->GetNumberOfColumns(); ++i)
    {
    d->PCA->SetColumnStatus(table->GetColumnName(i), 1);
    }
  d->PCA->RequestSelectedColumns();
  */

  //table->Print(std::cout);
 vtkSmartPointer<vtkTableToArray> tab = vtkSmartPointer<vtkTableToArray>::New();
 tab->SetInput(table);
#if 0
  VTK_ALLOCATE_MACRO(reader, vtkDelimitedTextReader);
  reader->SetInput(this->input()->data());
  reader->SetFieldDelimiterCharacters(",");
  reader->SetHaveHeaders(1);
  reader->DetectNumericColumnsOn();

  reader->Update();

  for (int ctr=4; ctr<28; ctr++)
    {
    tab->AddColumn(reader->GetOutput()->GetColumnName(ctr));
    }
  tab->Update();

#endif



  for (int ctr=1; ctr<table->GetNumberOfColumns(); ctr++)
    {
    tab->AddColumn(table->GetColumnName(ctr));
    }
  
  tab->Update();
 // tab->GetOutput()->Print(std::cout);

  d->PCA->SetRoutput(0);
  d->PCA->SetInputConnection(tab->GetOutputPort());
  d->PCA->PutArray( "0", "PCAData");
  d->PCA->SetRscript("pc1<-prcomp(PCAData, scale.=F, center=T, retx=T)\npcaRot<-pc1$rot \npcaSdev<-pc1$sdev\n data<-summary(pc1)\n numcol=ncol(data$importance)\n OutputData<-unlist(data[6],use.names=FALSE)\nstddev=OutputData[((1:numcol)*3)-2]\n perload=OutputData[((1:numcol)*3)-1]\n sumperload=OutputData[((1:numcol)*3)] \nprojection<-pc1$x");
  //d->PCA->SetRscript("output<-pca(PCAData[,2:10 ],method=\"svd\" "); [1:27,2:12] \npcaRot\npcaSdev  PCADataInit \n PCAData<-t(PCADataInit)\n
  //d->PCA->GetTable("output");
  d->PCA->GetArray("pcaRot","pcaRot");
  d->PCA->GetArray("pcaSdev","pcaSdev");
  d->PCA->GetArray("sumperload","sumperload");
  d->PCA->GetArray("perload","perload");
  d->PCA->GetArray("stddev","stddev");
  d->PCA->GetArray("projection","projection");

  // Do PCA
  //  d->PCA->SetInput(table);
  d->PCA->Update();

  //vtkMultiBlockDataSet* learn = vtkMultiBlockDataSet::SafeDownCast(d->PCA->GetOutputDataObject(1));

#if 0
  VTK_ALLOCATE_MACRO(pcaSdevData, vtkArrayData);
  pcaSdevData->AddArray(pcaReturn->GetArrayByName("pcaSdev"));

  VTK_ALLOCATE_MACRO(pcaSdev, vtkArrayToTable);
  pcaSdev->SetInputConnection(pcaSdevData->GetProducerPort());
  pcaSdev->Update();
  /*
  assess->Dump();
  for (unsigned int b = 0; b < learn->GetNumberOfBlocks(); ++b)
    {
    vtkTable::SafeDownCast(learn->GetBlock(b))->Dump();
    }
  */

  // Find standard deviations of each column
  d->Descriptive->ResetRequests();
  d->Descriptive->ResetAllColumnStates();
  for (int i = 1; i < table->GetNumberOfRows(); ++i)
    {
    d->Descriptive->SetColumnStatus(table->GetColumnName(i), 1);
    d->Descriptive->RequestSelectedColumns();
    d->Descriptive->ResetAllColumnStates();
    }

  // Do descriptive stats
  d->Descriptive->SetInput(table);
  d->Descriptive->Update();

  vtkMultiBlockDataSet* dBlock = vtkMultiBlockDataSet::SafeDownCast(d->Descriptive->GetOutputDataObject(1));
  vtkTable* descriptive = vtkTable::SafeDownCast(dBlock->GetBlock(1));
  vtkDoubleArray* columnDev = vtkDoubleArray::SafeDownCast(descriptive->GetColumnByName("Standard Deviation"));
#endif

  vtkArrayData *pcaReturn = vtkArrayData::SafeDownCast(d->PCA->GetOutput());
  if (!pcaReturn)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
    }

  // Set up headers for the rows.
  vtkIdType cols = table->GetNumberOfColumns() - 1;
  vtkIdType rows = table->GetNumberOfRows();
  vtkSmartPointer<vtkStringArray> header = vtkSmartPointer<vtkStringArray>::New();
  header->SetName("header");
  header->SetNumberOfTuples(rows);
  for (vtkIdType i = 0; i < rows; ++i)
    {
    header->SetValue(i, vtkVariant(i).ToString());
    }

  // Extract rotated coordinates
  vtkSmartPointer<vtkArrayData> pcaProjData = vtkSmartPointer<vtkArrayData>::New();
  pcaProjData->AddArray(pcaReturn->GetArrayByName("projection"));

  vtkSmartPointer<vtkArrayToTable> pcaProj = vtkSmartPointer<vtkArrayToTable>::New();
  pcaProj->SetInputConnection(pcaProjData->GetProducerPort());
  pcaProj->Update();  
  
  vtkTable* assess = vtkTable::SafeDownCast(pcaProj->GetOutput());
  vtkSmartPointer<vtkTable> xtab = vtkSmartPointer<vtkTable>::New();
  xtab->AddColumn(table->GetColumn(0));
  for (vtkIdType c = 0; c < assess->GetNumberOfRows(); ++c)
    {
    vtkAbstractArray* col = assess->GetColumn(c);
    col->SetName(vtkVariant(c).ToString());
    xtab->AddColumn(col);
		/*
		vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
    arr->SetName(table->GetColumnName(c + 1));
    arr->SetNumberOfTuples(rows);
    for (vtkIdType r = 0; r < rows; ++r)
      {
      arr->SetValue(r, assess->GetValue(r, c).ToDouble());
      }
    xtab->AddColumn(arr);
	*/
    }
  this->setOutput("x", new voTableDataObject("x", xtab));

  vtkSmartPointer<vtkArrayData> pcaRotData = vtkSmartPointer<vtkArrayData>::New();
  pcaRotData->AddArray(pcaReturn->GetArrayByName("pcaRot"));

  vtkSmartPointer<vtkArrayToTable> pcaRot = vtkSmartPointer<vtkArrayToTable>::New();
  pcaRot->SetInputConnection(pcaRotData->GetProducerPort());
  pcaRot->Update();
  
  vtkTable* compressed = vtkTable::SafeDownCast(pcaRot->GetOutput());
  vtkSmartPointer<vtkTable> rot = vtkSmartPointer<vtkTable>::New();
  // Extract rotation matrix (eigenvectors)

  rot->AddColumn(header);
  for (vtkIdType c = 0; c < cols; ++c)
    {
    vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
    arr->SetName(table->GetColumnName(c + 1));
    arr->SetNumberOfTuples(rows);
    for (vtkIdType r = 0; r < rows; ++r)
      {
      arr->SetValue(r, compressed->GetValue(c, r).ToDouble());
      }
    rot->AddColumn(arr);
    }

  this->setOutput("rot", new voTableDataObject("rot", rot));



  // Extract standard deviations for each principal component (eigenvalues)
  vtkSmartPointer<vtkArrayData> pcaStDevData = vtkSmartPointer<vtkArrayData>::New();
  pcaStDevData->AddArray(pcaReturn->GetArrayByName("stddev"));
  
  vtkSmartPointer<vtkArrayToTable> pcastdev = vtkSmartPointer<vtkArrayToTable>::New();
  pcastdev->SetInputConnection(pcaStDevData->GetProducerPort());
  pcastdev->Update();
  pcastdev->GetOutput()->Print(std::cout);

  vtkTable* marks =vtkTable::SafeDownCast(pcastdev->GetOutput());
  vtkSmartPointer<vtkTable> sdev = vtkSmartPointer<vtkTable>::New();
  sdev->AddColumn(header);
  vtkSmartPointer<vtkDoubleArray> sdevArr = vtkSmartPointer<vtkDoubleArray>::New();
  sdevArr->SetName("Std Dev");
  sdevArr->SetNumberOfTuples(rows);
  for (vtkIdType r = 0; r < rows; ++r)
    {
    sdevArr->SetValue(r,marks->GetValue(r,0).ToDouble());
    }
  sdev->AddColumn(sdevArr);
  this->setOutput("sdev", new voTableDataObject("sdev", sdev));



  //Extract the Variation values for percent loading analysis.

  vtkSmartPointer<vtkStringArray> perloadHeader = vtkSmartPointer<vtkStringArray>::New();

  vtkSmartPointer<vtkArrayData> pcaLoadingData = vtkSmartPointer<vtkArrayData>::New();
  pcaLoadingData->AddArray(pcaReturn->GetArrayByName("perload"));

  vtkSmartPointer<vtkArrayToTable> pcaLoad = vtkSmartPointer<vtkArrayToTable>::New();
  pcaLoad->SetInputConnection(pcaLoadingData->GetProducerPort());
  pcaLoad->Update();

  vtkTable* permarks =vtkTable::SafeDownCast(pcaLoad->GetOutput());
  vtkSmartPointer<vtkTable> loading = vtkSmartPointer<vtkTable>::New();
  loading->AddColumn(header);

  
  vtkSmartPointer<vtkDoubleArray> loadingArr = vtkSmartPointer<vtkDoubleArray>::New();
  vtkSmartPointer<vtkDoubleArray> loadingArr2 = vtkSmartPointer<vtkDoubleArray>::New();
  loadingArr->SetName("Percent Loading");
  loadingArr->SetNumberOfTuples(rows);   
  loadingArr2->SetName("Loading Vector");
  loadingArr2->SetNumberOfTuples(rows);   
  for (vtkIdType r = 0; r < rows; ++r)
    { 
    loadingArr->SetValue(r,permarks->GetValue(r,0).ToDouble());
	loadingArr2->SetValue(r,r);
    } 
  loading->AddColumn(loadingArr2);
  loading->AddColumn(loadingArr);
  this->setOutput("loading", new voTableDataObject("loading", loading));
  
  //Calculate the cumulative percent loading for the standard deviation.

    vtkSmartPointer<vtkArrayData> pcaSumLoadingData = vtkSmartPointer<vtkArrayData>::New();
  pcaSumLoadingData->AddArray(pcaReturn->GetArrayByName("sumperload"));

  vtkSmartPointer<vtkArrayToTable> pcaSumLoad = vtkSmartPointer<vtkArrayToTable>::New();
  pcaSumLoad->SetInputConnection(pcaSumLoadingData->GetProducerPort());
  pcaSumLoad->Update();

  vtkTable* sumpermarks =vtkTable::SafeDownCast(pcaSumLoad->GetOutput());
  vtkSmartPointer<vtkTable> sumloading = vtkSmartPointer<vtkTable>::New();
  sumloading->AddColumn(header);
  vtkSmartPointer<vtkDoubleArray> sumloadingArr = vtkSmartPointer<vtkDoubleArray>::New();
  sumloadingArr->SetName("Cumulative Percent Loading");
  sumloadingArr->SetNumberOfTuples(rows);
  for (vtkIdType r = 0; r < rows; ++r)
    {
    sumloadingArr->SetValue(r,sumpermarks->GetValue(r,0).ToDouble());
    }
  sumloading->AddColumn(loadingArr2);
  sumloading->AddColumn(sumloadingArr); 
  this->setOutput("sumloading", new voTableDataObject("sumloading", sumloading));
#if 0
  // Compute correlations as corr(A,B) = cov(A,B)/(stddev(A)*stddev(B))
  vtkSmartPointer<vtkTable> corr = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> corrHeader = vtkSmartPointer<vtkStringArray>::New();
  corrHeader->SetName("header");
  corrHeader->SetNumberOfTuples(cols);
  for (vtkIdType r = 0; r < cols; ++r)
    {
    corrHeader->SetValue(r, table->GetColumnName(r + 1));
    }
  corr->AddColumn(corrHeader);
  for (vtkIdType c = 0; c < cols; ++c)
    {
    vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
    arr->SetName(table->GetColumnName(c + 1));
    arr->SetNumberOfTuples(cols);                             //changed from rows
    double cdev = columnDev->GetValue(c);
    for (vtkIdType r = 0; r < rows; ++r)
      {
      double rdev = columnDev->GetValue(r);
      arr->SetValue(r, compressed->GetValue(r, c ).ToDouble()/(rdev*cdev));
      }
    for (vtkIdType r = c; r < rows; ++r)
      {
      double rdev = columnDev->GetValue(r);
      arr->SetValue(r, compressed->GetValue(c, r).ToDouble()/(rdev*cdev));
      }
    corr->AddColumn(arr);
    }
  this->setOutput("corr", new voTableDataObject("corr", corr));

  // Find high correlations to put in graph
  vtkSmartPointer<vtkTable> sparseCorr = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> col1 = vtkSmartPointer<vtkStringArray>::New();
  col1->SetName("Column 1");
  vtkSmartPointer<vtkStringArray> col2 = vtkSmartPointer<vtkStringArray>::New();
  col2->SetName("Column 2");
  vtkSmartPointer<vtkDoubleArray> valueArr = vtkSmartPointer<vtkDoubleArray>::New();
  valueArr->SetName("Correlation");
  for (vtkIdType r = 0; r < cols; ++r)
    {
    for (vtkIdType c = r+1; c < rows; ++c)
      {
      double val = corr->GetValue(r, c + 1).ToDouble();
      if (val > 0.3)
        {
        col1->InsertNextValue(table->GetColumnName(r + 1));
        col2->InsertNextValue(table->GetColumnName(c + 1));
        valueArr->InsertNextValue(val);
        }
      }
    }
  sparseCorr->AddColumn(col1);
  sparseCorr->AddColumn(col2);
  sparseCorr->AddColumn(valueArr);

  // Build the graph
  vtkSmartPointer<vtkTableToGraph> correlGraphAlg =
    vtkSmartPointer<vtkTableToGraph>::New();
  correlGraphAlg->SetInput(sparseCorr);
  correlGraphAlg->AddLinkVertex("Column 1");
  correlGraphAlg->AddLinkVertex("Column 2");
  correlGraphAlg->AddLinkEdge("Column 1", "Column 2");
  correlGraphAlg->Update();

  this->setOutput(
      "correlation graph", new voDataObject("correlation graph", correlGraphAlg->GetOutput()));
#endif
  return true;
}
