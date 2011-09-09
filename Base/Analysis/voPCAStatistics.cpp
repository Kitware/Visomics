
// Qt includes
#include <QDebug>

// Visomics includes
#include "voPCAStatistics.h"
#include "voTableDataObject.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayToTable.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
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
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voPCAStatistics methods

// --------------------------------------------------------------------------
voPCAStatistics::voPCAStatistics():
    Superclass(), d_ptr(new voPCAStatisticsPrivate)
{
  Q_D(voPCAStatistics);
  
  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
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
                      "voPCAProjectionView", "Projection (Plot)",
                      "voTableView", "Projection (Table)");

  this->addOutputType("rot", "vtkTable",
                      "",  "",
                      "voTableView", "Rotation (Table)");

  this->addOutputType("sdev", "vtkTable",
                      "", "",
                      "voTableView", "Std. Deviation (Table)");
    
  this->addOutputType("loading", "vtkTable" ,
                      "voPCABarView", "Percent Loading (Plot)",
                      "voTableView", "Percent Loading (Table)");
   
  this->addOutputType("sumloading", "vtkTable" ,
                      "voPCABarView", "Cumulative Percent Loading (Plot)",
                      "voTableView", "Cumulative Percent Loading (Table)");

  this->addOutputType("x-dynview", "vtkTable",
                      "voPCAProjectionDynView", "Projection (Interactive Plot)",
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

  vtkNew<vtkTableToArray> tab;
  tab->SetInput(table);

  for (int ctr=1; ctr<table->GetNumberOfColumns(); ctr++)
    {
    tab->AddColumn(table->GetColumnName(ctr));
    }
  
  tab->Update();

  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputConnection(tab->GetOutputPort());
  d->RCalc->PutArray("0", "PCAData");
  d->RCalc->SetRscript("pc1<-prcomp(t(PCAData), scale.=F, center=T, retx=T)\n"
                     "pcaRot<-pc1$rot\n"
                     "pcaSdev<-pc1$sdev\n"
                     "data<-summary(pc1)\n"
                     "numcol=ncol(data$importance)\n"
                     "OutputData<-unlist(data[6],use.names=FALSE)\n"
                     "stddev=OutputData[((1:numcol)*3)-2]\n"
                     "perload=OutputData[((1:numcol)*3)-1]\n"
                     "sumperload=OutputData[((1:numcol)*3)] \n"
                     "projection<-pc1$x");
  d->RCalc->GetArray("pcaRot", "pcaRot");
  d->RCalc->GetArray("pcaSdev", "pcaSdev");
  d->RCalc->GetArray("sumperload", "sumperload");
  d->RCalc->GetArray("perload", "perload");
  d->RCalc->GetArray("stddev", "stddev");
  d->RCalc->GetArray("projection", "projection");

  // Do PCA
  d->RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());
  if(!outputArrayData /* || outputArrayData->GetArrayByName("RerrValue")->GetVariantValue(0).ToInt() > 1*/)
    {
    qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
    return false;
    }

  // Set up headers for the rows.
  vtkSmartPointer<vtkStringArray> rowHeader = vtkStringArray::SafeDownCast(table->GetColumn(0));
  if (!rowHeader)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
    }
  vtkNew<vtkStringArray> colHeader;
  for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
    {
    colHeader->InsertNextValue(table->GetColumnName(c));
    }

  // Extract rotated coordinates (for projection plot)
  vtkNew<vtkArrayData> pcaProjData;
  pcaProjData->AddArray(outputArrayData->GetArrayByName("projection"));

  vtkNew<vtkArrayToTable> pcaProj;
  pcaProj->SetInputConnection(pcaProjData->GetProducerPort());
  pcaProj->Update();  
  
  vtkTable* assess = vtkTable::SafeDownCast(pcaProj->GetOutput());

  vtkNew<vtkTable> xtab;
  vtkNew<vtkStringArray> PCHeaderArr;
  for (vtkIdType c = 0; c < assess->GetNumberOfColumns(); ++c)
    {
    PCHeaderArr->InsertNextValue(QString("PC%1").arg(c + 1).toLatin1());
    }
  xtab->AddColumn(PCHeaderArr.GetPointer());
  for (vtkIdType r = 0; r < assess->GetNumberOfRows(); ++r)
    {
    vtkNew<vtkDoubleArray> arr;
    arr->SetName(colHeader->GetValue(r));
    arr->SetNumberOfTuples(assess->GetNumberOfColumns());
    for (vtkIdType c = 0; c < assess->GetNumberOfColumns(); ++c)
      {
      arr->SetValue(c, assess->GetValue(r, c).ToDouble());
      }
    xtab->AddColumn(arr.GetPointer());
    }

  this->setOutput("x", new voTableDataObject("x", xtab.GetPointer()));
  this->setOutput("x-dynview", new voTableDataObject("x-dynview", xtab.GetPointer()));


  // Extract rotation matrix (each column is an eigenvector)
  vtkNew<vtkArrayData> pcaRotData;
  pcaRotData->AddArray(outputArrayData->GetArrayByName("pcaRot"));

  vtkNew<vtkArrayToTable> pcaRot;
  pcaRot->SetInputConnection(pcaRotData->GetProducerPort());
  pcaRot->Update();
  
  vtkTable* compressed = vtkTable::SafeDownCast(pcaRot->GetOutput());

  vtkNew<vtkTable> rot;
  rot->AddColumn(rowHeader);
  for (vtkIdType c = 0;c < compressed->GetNumberOfColumns(); ++c)
    {
    vtkAbstractArray* col = compressed->GetColumn(c);
    col->SetName(QString("PC%1").arg(c + 1).toLatin1());
    rot->AddColumn(col);
    }
  this->setOutput("rot", new voTableDataObject("rot", rot.GetPointer()));


  // Extract standard deviations for each principal component (eigenvalues)
  vtkNew<vtkArrayData> pcaStDevData;
  pcaStDevData->AddArray(outputArrayData->GetArrayByName("stddev"));
  
  vtkNew<vtkArrayToTable> pcastdev;
  pcastdev->SetInputConnection(pcaStDevData->GetProducerPort());
  pcastdev->Update();

  vtkTable* marks = vtkTable::SafeDownCast(pcastdev->GetOutput());

  vtkNew<vtkTable> sdev;
  vtkNew<vtkStringArray> sdevHeaderArray;
  sdevHeaderArray->InsertNextValue("Std Dev");
  sdev->AddColumn(sdevHeaderArray.GetPointer());
  for (vtkIdType r = 0;r < marks->GetNumberOfRows(); ++r)
    {
    vtkNew<vtkDoubleArray> sdevArr;
    sdevArr->SetName(QString("PC%1").arg(r + 1).toLatin1());
    sdevArr->InsertNextValue(marks->GetValue(r,0).ToDouble());
    sdev->AddColumn(sdevArr.GetPointer());
    }
  this->setOutput("sdev", new voTableDataObject("sdev", sdev.GetPointer()));


  //Extract the Variation values for percent loading (proportion of variance) analysis
  vtkNew<vtkArrayData> pcaLoadingData;
  pcaLoadingData->AddArray(outputArrayData->GetArrayByName("perload"));

  vtkNew<vtkArrayToTable> pcaLoad;
  pcaLoad->SetInputConnection(pcaLoadingData->GetProducerPort());
  pcaLoad->Update();

  vtkTable* permarks =vtkTable::SafeDownCast(pcaLoad->GetOutput());

  vtkNew<vtkTable> loading;
  vtkNew<vtkStringArray> loadingHeaderArray;
  loadingHeaderArray->InsertNextValue("Loading Vector");
  loadingHeaderArray->InsertNextValue("Percent Loading");
  loading->AddColumn(loadingHeaderArray.GetPointer());

  for (vtkIdType r = 0;r < permarks->GetNumberOfRows(); ++r)
    {
    vtkNew<vtkDoubleArray> loadingArr;
    loadingArr->SetName(QString("PC%1").arg(r + 1).toLatin1());
    loadingArr->InsertNextValue(r);
    loadingArr->InsertNextValue(permarks->GetValue(r,0).ToDouble());
    loading->AddColumn(loadingArr.GetPointer());
    }
  this->setOutput("loading", new voTableDataObject("loading", loading.GetPointer()));


  //Calculate the cumulative percent loading for the standard deviation.
  vtkNew<vtkArrayData> pcaSumLoadingData;
  pcaSumLoadingData->AddArray(outputArrayData->GetArrayByName("sumperload"));

  vtkNew<vtkArrayToTable> pcaSumLoad;
  pcaSumLoad->SetInputConnection(pcaSumLoadingData->GetProducerPort());
  pcaSumLoad->Update();

  vtkTable* sumpermarks = vtkTable::SafeDownCast(pcaSumLoad->GetOutput());

  vtkNew<vtkTable> sumloading;
  vtkNew<vtkStringArray> sumloadingHeaderArray;
  sumloadingHeaderArray->InsertNextValue("Loading Vector");
  sumloadingHeaderArray->InsertNextValue("Cumulative Percent Loading");
  sumloading->AddColumn(sumloadingHeaderArray.GetPointer());

  for (vtkIdType r = 0;r < sumpermarks->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkDoubleArray> sumloadingArr = vtkSmartPointer<vtkDoubleArray>::New();
    sumloadingArr->SetName(QString("PC%1").arg(r + 1).toLatin1());
    sumloadingArr->InsertNextValue(r);
    sumloadingArr->InsertNextValue(sumpermarks->GetValue(r,0).ToDouble());
    sumloading->AddColumn(sumloadingArr.GetPointer());
    }
  this->setOutput("sumloading", new voTableDataObject("sumloading", sumloading.GetPointer()));

  return true;
}
