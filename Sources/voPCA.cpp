
// Visomics includes
#include "voPCA.h"
#include "voCorrelationGraphView.h"
#include "voPCAProjectionPlot.h"
#include "voTableView.h"

// VTK includes
#include <vtkAdjacencyMatrixToEdgeTable.h>
#include <vtkAlgorithm.h>
#include <vtkArrayToTable.h>
#include <vtkDataSetAttributes.h>
#include <vtkGraph.h>
#include <vtkRCalculatorFilter.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkTableToGraph.h>

// --------------------------------------------------------------------------
voPCA::voPCA()
{
  vtkRCalculatorFilter* calc = vtkRCalculatorFilter::New();
  calc->SetRoutput(0);
  calc->PutArray("metabData", "metabData");
  calc->GetArray("pcaRot","pcaRot");
  calc->GetArray("pcaSdev","pcaSdev");
  calc->GetArray("correl","correl");
  calc->GetArray("pcaX","pcaX");
  calc->SetRscript("pc1<-prcomp(metabData, scale.=T, center=F) \n"
                   " pcaRot<-pc1$rot \n"
                   " pcaSdev<-pc1$sdev \n"
                   " pcaX<-pc1$x \n"
                   " correl<-cor(metabData)");

  this->Algorithm = calc;

  voCorrelationGraphView* correl = new voCorrelationGraphView();
  correl->setInput(voPort(this, "correlation graph"));
  this->Views["Correlation Graph"] = correl;

  voPCAProjectionPlot* plot = new voPCAProjectionPlot();
  plot->setInput(voPort(this, "x"));
  this->Views["Projection Plot"] = plot;

  this->Views["Table (Coordinates)"] = new voTableView();
  this->Views["Table (Coordinates)"]->setInput(voPort(this, "x"));

  this->Views["Table (Rotation)"] = new voTableView();
  this->Views["Table (Rotation)"]->setInput(voPort(this, "rot"));

  this->Views["Table (Std. Deviation)"] = new voTableView();
  this->Views["Table (Std. Deviation)"]->setInput(voPort(this, "sdev"));
}

// --------------------------------------------------------------------------
void voPCA::updateInternal()
{
  vtkTable* table = vtkTable::SafeDownCast(this->input().data());
  if (!table)
    {
    return;
    }

  vtkSmartPointer<vtkTableToArray> tab =
    vtkSmartPointer<vtkTableToArray>::New();
  tab->SetInput(table);

  vtkSmartPointer<vtkStringArray> names =
    vtkSmartPointer<vtkStringArray>::New();
  names->SetName("Metabolites");

  // Add all numeric columns
  for (vtkIdType column = 0; column < table->GetNumberOfColumns(); ++column)
    {
    if (vtkDataArray::SafeDownCast(table->GetColumn(column)))
      {
      tab->AddColumn(table->GetColumnName(column));
      names->InsertNextValue(table->GetColumnName(column));
      }
    }

  this->Algorithm->SetInputConnection(tab->GetOutputPort());
  this->Algorithm->Update();

  vtkArrayData *temp = vtkArrayData::SafeDownCast(this->Algorithm->GetOutputDataObject(0));
  if (!temp)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return;
    }

  vtkSmartPointer<vtkArrayData> pcaReturn =
    vtkSmartPointer<vtkArrayData>::New();
  pcaReturn->DeepCopy(temp);

  vtkSmartPointer<vtkArrayData> pcaXData =
    vtkSmartPointer<vtkArrayData>::New();
  pcaXData->AddArray(pcaReturn->GetArrayByName("pcaX"));
  vtkSmartPointer<vtkArrayToTable> pcaX =
    vtkSmartPointer<vtkArrayToTable>::New();
  pcaX->SetInputConnection(pcaXData->GetProducerPort());
  pcaX->Update();
  vtkSmartPointer<vtkTable> pcaXTable =
    vtkSmartPointer<vtkTable>::New();
  pcaXTable->ShallowCopy(pcaX->GetOutput());
  for (vtkIdType col = 0; col < table->GetNumberOfColumns(); ++col)
    {
    pcaXTable->AddColumn(table->GetColumn(col));
    }
  this->Outputs["x"] = pcaXTable;

  vtkSmartPointer<vtkArrayData> pcaRotData =
    vtkSmartPointer<vtkArrayData>::New();
  pcaRotData->AddArray(pcaReturn->GetArrayByName("pcaRot"));
  vtkSmartPointer<vtkArrayToTable> pcaRot =
    vtkSmartPointer<vtkArrayToTable>::New();
  pcaRot->SetInputConnection(pcaRotData->GetProducerPort());
  pcaRot->Update();
  this->Outputs["rot"] = pcaRot->GetOutput();

  vtkSmartPointer<vtkArrayData> pcaSdevData =
    vtkSmartPointer<vtkArrayData>::New();
  pcaSdevData->AddArray(pcaReturn->GetArrayByName("pcaSdev"));
  vtkSmartPointer<vtkArrayToTable> pcaSdev =
    vtkSmartPointer<vtkArrayToTable>::New();
  pcaSdev->SetInputConnection(pcaSdevData->GetProducerPort());
  pcaSdev->Update();
  this->Outputs["sdev"] = pcaSdev->GetOutput();

  pcaReturn->GetArrayByName("correl")->SetDimensionLabel(0, "Metabolite 0");
  pcaReturn->GetArrayByName("correl")->SetDimensionLabel(1, "Metabolite 1");

  vtkSmartPointer<vtkArrayData> correlData =
    vtkSmartPointer<vtkArrayData>::New();
  correlData->AddArray(pcaReturn->GetArrayByName("correl"));

  vtkSmartPointer<vtkAdjacencyMatrixToEdgeTable> correl =
    vtkSmartPointer<vtkAdjacencyMatrixToEdgeTable>::New();
  correl->SetInputConnection(correlData->GetProducerPort());
  correl->SetValueArrayName("Correlation");
  correl->Update();

  vtkSmartPointer<vtkTableToGraph> correlGraphAlg =
    vtkSmartPointer<vtkTableToGraph>::New();
  correlGraphAlg->SetInputConnection(correl->GetOutputPort());
  correlGraphAlg->AddLinkVertex("Metabolite 0");
  correlGraphAlg->AddLinkVertex("Metabolite 1");
  correlGraphAlg->AddLinkEdge("Metabolite 0", "Metabolite 1");
  correlGraphAlg->Update();

  vtkSmartPointer<vtkGraph> correlGraph;
  correlGraph.TakeReference(correlGraphAlg->GetOutput()->NewInstance());
  correlGraph->DeepCopy(correlGraphAlg->GetOutput());
  correlGraph->GetVertexData()->AddArray(names);
  this->Outputs["correlation graph"] = correlGraph;
}
