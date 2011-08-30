
// Qt includes
#include <QDebug>
#include <QLayout>

// Visomics includes
#include "voPCAStatistics.h"
#include "voTableDataObject.h"
#include "vtkExtendedTable.h"

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
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voPCAStatistics::setOutputInformation()
{
  this->addOutputType("x", "vtkTable",
                      "voPCAProjectionView", "Projection Plot",
                      "voTableView", "Table (Projection Plot)");

  this->addOutputType("rot", "vtkTable",
                      "",  "",
                      "voTableView", "Table (Rotation)");

  this->addOutputType("sdev", "vtkTable",
                      "", "",
                      "voTableView", "Table (Std. Deviation)");
    
  this->addOutputType("loading", "vtkTable" ,
                      "voPCABarView", "Percent Loading Plot",
                      "voTableView", "Table (Percent Loading Plot)");
   
  this->addOutputType("sumloading", "vtkTable" ,
                      "voPCABarView", "Cumulative Percent Loading Plot",
                      "voTableView", "Table (Cumulative Percent Loading Plot)");

  this->addOutputType("x-dynview", "vtkTable",
                      "voPCAProjectionDynView", "Interactive Projection Plot (prototype)",
                      "", "");
}

// --------------------------------------------------------------------------
bool voPCAStatistics::execute()
{
  Q_D(voPCAStatistics);

  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qWarning() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::Take(extendedTable->GetDataWithRowHeader());

  vtkSmartPointer<vtkTableToArray> tab = vtkSmartPointer<vtkTableToArray>::New();
  tab->SetInput(table);

  for (int ctr=1; ctr<table->GetNumberOfColumns(); ctr++)
    {
    tab->AddColumn(table->GetColumnName(ctr));
    }
  
  tab->Update();

  d->PCA->SetRoutput(0);
  d->PCA->SetInputConnection(tab->GetOutputPort());
  d->PCA->PutArray("0", "PCAData");
  d->PCA->SetRscript("pc1<-prcomp(t(PCAData), scale.=F, center=T, retx=T)\n"
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
  vtkSmartPointer<vtkStringArray> rowHeader = vtkStringArray::SafeDownCast(table->GetColumn(0));
  if (!rowHeader)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
    }
  vtkSmartPointer<vtkStringArray> colHeader = vtkSmartPointer<vtkStringArray>::New();
  for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
    {
    colHeader->InsertNextValue(table->GetColumnName(c));
    }

  // Extract rotated coordinates (for projection plot)
  vtkSmartPointer<vtkArrayData> pcaProjData = vtkSmartPointer<vtkArrayData>::New();
  pcaProjData->AddArray(pcaReturn->GetArrayByName("projection"));

  vtkSmartPointer<vtkArrayToTable> pcaProj = vtkSmartPointer<vtkArrayToTable>::New();
  pcaProj->SetInputConnection(pcaProjData->GetProducerPort());
  pcaProj->Update();  
  
  vtkTable* assess = vtkTable::SafeDownCast(pcaProj->GetOutput());

  vtkSmartPointer<vtkTable> xtab = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> PCHeaderArr = vtkSmartPointer<vtkStringArray>::New();
  for (vtkIdType c = 0; c < assess->GetNumberOfColumns(); ++c)
    {
    PCHeaderArr->InsertNextValue(QString("PC%1").arg(c + 1).toLatin1());
	//std::cout << PCHeaderArr[1,c+1] << std::endl;
    }
  xtab->AddColumn(PCHeaderArr);
  for (vtkIdType r = 0; r < assess->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
    arr->SetName(colHeader->GetValue(r));
    arr->SetNumberOfTuples(assess->GetNumberOfColumns());
    for (vtkIdType c = 0; c < assess->GetNumberOfColumns(); ++c)
      {
      arr->SetValue(c, assess->GetValue(r, c).ToDouble()); 
      }
    xtab->AddColumn(arr);
    }

  this->setOutput("x", new voTableDataObject("x", xtab));
  this->setOutput("x-dynview", new voTableDataObject("x-dynview", xtab));


  // Extract rotation matrix (each column is an eigenvector)
  vtkSmartPointer<vtkArrayData> pcaRotData = vtkSmartPointer<vtkArrayData>::New();
  pcaRotData->AddArray(pcaReturn->GetArrayByName("pcaRot"));

  vtkSmartPointer<vtkArrayToTable> pcaRot = vtkSmartPointer<vtkArrayToTable>::New();
  pcaRot->SetInputConnection(pcaRotData->GetProducerPort());
  pcaRot->Update();
  
  vtkTable* compressed = vtkTable::SafeDownCast(pcaRot->GetOutput());

  vtkSmartPointer<vtkTable> rot = vtkSmartPointer<vtkTable>::New();
  rot->AddColumn(rowHeader);
  for (vtkIdType c = 0;c < compressed->GetNumberOfColumns(); ++c)
    {
    vtkAbstractArray* col = compressed->GetColumn(c);
    col->SetName(QString("PC%1").arg(c + 1).toLatin1());
    rot->AddColumn(col);
    }
  this->setOutput("rot", new voTableDataObject("rot", rot));


  // Extract standard deviations for each principal component (eigenvalues)
  vtkSmartPointer<vtkArrayData> pcaStDevData = vtkSmartPointer<vtkArrayData>::New();
  pcaStDevData->AddArray(pcaReturn->GetArrayByName("stddev"));
  
  vtkSmartPointer<vtkArrayToTable> pcastdev = vtkSmartPointer<vtkArrayToTable>::New();
  pcastdev->SetInputConnection(pcaStDevData->GetProducerPort());
  pcastdev->Update();

  vtkTable* marks = vtkTable::SafeDownCast(pcastdev->GetOutput());

  vtkSmartPointer<vtkTable> sdev = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> sdevHeaderArray = vtkSmartPointer<vtkStringArray>::New();
  sdevHeaderArray->InsertNextValue("Std Dev");
  sdev->AddColumn(sdevHeaderArray);
  for (vtkIdType r = 0;r < marks->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkDoubleArray> sdevArr = vtkSmartPointer<vtkDoubleArray>::New();
    sdevArr->SetName(QString("PC%1").arg(r + 1).toLatin1());
    sdevArr->InsertNextValue(marks->GetValue(r,0).ToDouble());
    sdev->AddColumn(sdevArr);
    }
  this->setOutput("sdev", new voTableDataObject("sdev", sdev));


  //Extract the Variation values for percent loading (proportion of variance) analysis
  vtkSmartPointer<vtkArrayData> pcaLoadingData = vtkSmartPointer<vtkArrayData>::New();
  pcaLoadingData->AddArray(pcaReturn->GetArrayByName("perload"));

  vtkSmartPointer<vtkArrayToTable> pcaLoad = vtkSmartPointer<vtkArrayToTable>::New();
  pcaLoad->SetInputConnection(pcaLoadingData->GetProducerPort());
  pcaLoad->Update();

  vtkTable* permarks =vtkTable::SafeDownCast(pcaLoad->GetOutput());

  vtkSmartPointer<vtkTable> loading = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> loadingHeaderArray = vtkSmartPointer<vtkStringArray>::New();
  loadingHeaderArray->InsertNextValue("Loading Vector");
  loadingHeaderArray->InsertNextValue("Percent Loading");
  loading->AddColumn(loadingHeaderArray);

  for (vtkIdType r = 0;r < permarks->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkDoubleArray> loadingArr = vtkSmartPointer<vtkDoubleArray>::New();
    loadingArr->SetName(QString("PC%1").arg(r + 1).toLatin1());
    loadingArr->InsertNextValue(r);
    loadingArr->InsertNextValue(permarks->GetValue(r,0).ToDouble());
    loading->AddColumn(loadingArr);
    }
  this->setOutput("loading", new voTableDataObject("loading", loading));


  //Calculate the cumulative percent loading for the standard deviation.
  vtkSmartPointer<vtkArrayData> pcaSumLoadingData = vtkSmartPointer<vtkArrayData>::New();
  pcaSumLoadingData->AddArray(pcaReturn->GetArrayByName("sumperload"));

  vtkSmartPointer<vtkArrayToTable> pcaSumLoad = vtkSmartPointer<vtkArrayToTable>::New();
  pcaSumLoad->SetInputConnection(pcaSumLoadingData->GetProducerPort());
  pcaSumLoad->Update();

  vtkTable* sumpermarks = vtkTable::SafeDownCast(pcaSumLoad->GetOutput());

  vtkSmartPointer<vtkTable> sumloading = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> sumloadingHeaderArray = vtkSmartPointer<vtkStringArray>::New();
  sumloadingHeaderArray->InsertNextValue("Loading Vector");
  sumloadingHeaderArray->InsertNextValue("Cumulative Percent Loading");
  sumloading->AddColumn(sumloadingHeaderArray);

  for (vtkIdType r = 0;r < sumpermarks->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkDoubleArray> sumloadingArr = vtkSmartPointer<vtkDoubleArray>::New();
    sumloadingArr->SetName(QString("PC%1").arg(r + 1).toLatin1());
    sumloadingArr->InsertNextValue(r);
    sumloadingArr->InsertNextValue(sumpermarks->GetValue(r,0).ToDouble());
    sumloading->AddColumn(sumloadingArr);
    }
  this->setOutput("sumloading", new voTableDataObject("sumloading", sumloading));

  return true;
}
