
// Qt includes
#include <QDebug>

// Visomics includes
#include "voPCAStatistics.h"
#include "voTableDataObject.h"

// VTK includes
#include <vtkArrayToTable.h>
#include <vtkDoubleArray.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>

// --------------------------------------------------------------------------
// voPCAStatisticsPrivate methods

// --------------------------------------------------------------------------
class voPCAStatisticsPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> PCA;
};

// --------------------------------------------------------------------------
// voPCAStatistics methods

// --------------------------------------------------------------------------
voPCAStatistics::voPCAStatistics():
    Superclass(), d_ptr(new voPCAStatisticsPrivate)
{
  Q_D(voPCAStatistics);
  
  d->PCA = vtkSmartPointer<vtkRCalculatorFilter>::New();
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
                      "voTableView", "Table (Cumulative Percent Loading Plot)");
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

  //table->Print(std::cout);
  vtkSmartPointer<vtkTableToArray> tab = vtkSmartPointer<vtkTableToArray>::New();
  tab->SetInput(table);

  for (int ctr=1; ctr<table->GetNumberOfColumns(); ctr++)
    {
    tab->AddColumn(table->GetColumnName(ctr));
    }
  
  tab->Update();
 // tab->GetOutput()->Print(std::cout);

  d->PCA->SetRoutput(0);
  d->PCA->SetInputConnection(tab->GetOutputPort());
  d->PCA->PutArray("0", "PCAData");
  d->PCA->SetRscript("pc1<-prcomp(PCAData, scale.=F, center=T, retx=T)\n"
                     "pcaRot<-pc1$rot\n"
                     "pcaSdev<-pc1$sdev\n"
                     "data<-summary(pc1)\n"
                     "numcol=ncol(data$importance)\n"
                     "OutputData<-unlist(data[6],use.names=FALSE)\n"
                     "stddev=OutputData[((1:numcol)*3)-2]\n"
                     "perload=OutputData[((1:numcol)*3)-1]\n"
                     "sumperload=OutputData[((1:numcol)*3)] \n"
                     "projection<-pc1$x");
  d->PCA->GetArray("pcaRot", "pcaRot");
  d->PCA->GetArray("pcaSdev", "pcaSdev");
  d->PCA->GetArray("sumperload", "sumperload");
  d->PCA->GetArray("perload", "perload");
  d->PCA->GetArray("stddev", "stddev");
  d->PCA->GetArray("projection", "projection");

  // Do PCA
  d->PCA->Update();

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
    if (col)
      {
      col->SetName(vtkVariant(c).ToString());
      xtab->AddColumn(col);
      }
    }
  this->setOutput("x", new voTableDataObject("x", xtab));

  vtkSmartPointer<vtkArrayData> pcaRotData = vtkSmartPointer<vtkArrayData>::New();
  pcaRotData->AddArray(pcaReturn->GetArrayByName("pcaRot"));

  vtkSmartPointer<vtkArrayToTable> pcaRot = vtkSmartPointer<vtkArrayToTable>::New();
  pcaRot->SetInputConnection(pcaRotData->GetProducerPort());
  pcaRot->Update();
  
  // Extract rotation matrix (eigenvectors)
  vtkTable* compressed = vtkTable::SafeDownCast(pcaRot->GetOutput());
  vtkSmartPointer<vtkTable> rot = vtkSmartPointer<vtkTable>::New();
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
  //pcastdev->GetOutput()->Print(std::cout);

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

  vtkTable* sumpermarks = vtkTable::SafeDownCast(pcaSumLoad->GetOutput());
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

  return true;
}
