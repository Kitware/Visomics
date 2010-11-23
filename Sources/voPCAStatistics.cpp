
// Visomics includes
#include "voPCAStatistics.h"
#include "voCorrelationGraphView.h"
#include "voPCAProjectionPlot.h"
#include "voTableView.h"

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
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkTableToGraph.h>

// --------------------------------------------------------------------------
voPCAStatistics::voPCAStatistics()
{
  this->PCA = vtkSmartPointer<vtkPCAStatistics>::New();
  this->PCA->SetLearnOption(true);
  this->PCA->SetDeriveOption(true);
  this->PCA->SetAssessOption(true);
  this->PCA->SetBasisSchemeByName("FixedBasisEnergy");
  this->PCA->SetFixedBasisEnergy(0.95);

  this->Descriptive = vtkSmartPointer<vtkDescriptiveStatistics>::New();

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

  this->Views["Table (Correlation)"] = new voTableView();
  this->Views["Table (Correlation)"]->setInput(voPort(this, "corr"));
}

// --------------------------------------------------------------------------
void voPCAStatistics::updateInternal()
{
  vtkTable* table = vtkTable::SafeDownCast(this->input().data());
  if (!table)
    {
    return;
    }

  // Add request to process all columns
  this->PCA->ResetRequests();
  this->PCA->ResetAllColumnStates();
  for (int i = 1; i < table->GetNumberOfColumns(); ++i)
    {
    this->PCA->SetColumnStatus(table->GetColumnName(i), 1);
    }
  this->PCA->RequestSelectedColumns();

  // Do PCA
  this->PCA->SetInput(table);
  this->PCA->Update();

  vtkTable* assess = vtkTable::SafeDownCast(this->PCA->GetOutputDataObject(0));
  vtkMultiBlockDataSet* learn = vtkMultiBlockDataSet::SafeDownCast(this->PCA->GetOutputDataObject(1));

  /*
  assess->Dump();
  for (unsigned int b = 0; b < learn->GetNumberOfBlocks(); ++b)
    {
    vtkTable::SafeDownCast(learn->GetBlock(b))->Dump();
    }
  */

  // Find standard deviations of each column
  this->Descriptive->ResetRequests();
  this->Descriptive->ResetAllColumnStates();
  for (int i = 1; i < table->GetNumberOfColumns(); ++i)
    {
    this->Descriptive->SetColumnStatus(table->GetColumnName(i), 1);
    this->Descriptive->RequestSelectedColumns();
    this->Descriptive->ResetAllColumnStates();
    }

  // Do descriptive stats
  this->Descriptive->SetInput(table);
  this->Descriptive->Update();

  vtkMultiBlockDataSet* dBlock = vtkMultiBlockDataSet::SafeDownCast(this->Descriptive->GetOutputDataObject(1));
  vtkTable* descriptive = vtkTable::SafeDownCast(dBlock->GetBlock(1));
  vtkDoubleArray* columnDev = vtkDoubleArray::SafeDownCast(descriptive->GetColumnByName("Standard Deviation"));

  // Extract rotated coordinates
  vtkSmartPointer<vtkTable> xtab = vtkSmartPointer<vtkTable>::New();
  xtab->AddColumn(assess->GetColumn(0));
  for (vtkIdType c = table->GetNumberOfColumns(); c < assess->GetNumberOfColumns(); ++c)
    {
    vtkAbstractArray* col = assess->GetColumn(c);
    col->SetName(vtkVariant(c-table->GetNumberOfColumns()).ToString());
    xtab->AddColumn(col);
    }
  this->Outputs["x"] = xtab;

  vtkTable* compressed = vtkTable::SafeDownCast(learn->GetBlock(1));
  vtkSmartPointer<vtkTable> rot = vtkSmartPointer<vtkTable>::New();
  vtkIdType n = table->GetNumberOfColumns() - 1;

  // Extract rotation matrix (eigenvectors)
  vtkSmartPointer<vtkStringArray> rotHeader = vtkSmartPointer<vtkStringArray>::New();
  rotHeader->SetName("header");
  rotHeader->SetNumberOfTuples(n);
  for (vtkIdType i = 0; i < n; ++i)
    {
    rotHeader->SetValue(i, vtkVariant(i).ToString());
    }
  rot->AddColumn(rotHeader);
  for (vtkIdType c = 0; c < n; ++c)
    {
    vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
    arr->SetName(table->GetColumnName(c + 1));
    arr->SetNumberOfTuples(n);
    for (vtkIdType r = 0; r < n; ++r)
      {
      arr->SetValue(r, compressed->GetValue(r + n + 1, c + 2).ToDouble());
      }
    rot->AddColumn(arr);
    }
  this->Outputs["rot"] = rot;

  // Extract standard deviations for each principal component (eigenvalues)
  vtkSmartPointer<vtkTable> sdev = vtkSmartPointer<vtkTable>::New();
  sdev->AddColumn(rotHeader);
  vtkSmartPointer<vtkDoubleArray> sdevArr = vtkSmartPointer<vtkDoubleArray>::New();
  sdevArr->SetName("Std Dev");
  sdevArr->SetNumberOfTuples(n);
  for (vtkIdType r = 0; r < n; ++r)
    {
    sdevArr->SetValue(r, compressed->GetValue(r + n + 1, 1).ToDouble());
    }
  sdev->AddColumn(sdevArr);
  this->Outputs["sdev"] = sdev;

  // Compute correlations as corr(A,B) = cov(A,B)/(stddev(A)*stddev(B))
  vtkSmartPointer<vtkTable> corr = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> corrHeader = vtkSmartPointer<vtkStringArray>::New();
  corrHeader->SetName("header");
  corrHeader->SetNumberOfTuples(n);
  for (vtkIdType r = 0; r < n; ++r)
    {
    corrHeader->SetValue(r, table->GetColumnName(r + 1));
    }
  corr->AddColumn(corrHeader);
  for (vtkIdType c = 0; c < n; ++c)
    {
    vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
    arr->SetName(table->GetColumnName(c + 1));
    arr->SetNumberOfTuples(n);
    double cdev = columnDev->GetValue(c);
    for (vtkIdType r = 0; r < c; ++r)
      {
      double rdev = columnDev->GetValue(r);
      arr->SetValue(r, compressed->GetValue(r, c + 2).ToDouble()/(rdev*cdev));
      }
    for (vtkIdType r = c; r < n; ++r)
      {
      double rdev = columnDev->GetValue(r);
      arr->SetValue(r, compressed->GetValue(c, r + 2).ToDouble()/(rdev*cdev));
      }
    corr->AddColumn(arr);
    }
  this->Outputs["corr"] = corr;

  // Find high correlations to put in graph
  vtkSmartPointer<vtkTable> sparseCorr = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> col1 = vtkSmartPointer<vtkStringArray>::New();
  col1->SetName("Column 1");
  vtkSmartPointer<vtkStringArray> col2 = vtkSmartPointer<vtkStringArray>::New();
  col2->SetName("Column 2");
  vtkSmartPointer<vtkDoubleArray> valueArr = vtkSmartPointer<vtkDoubleArray>::New();
  valueArr->SetName("Correlation");
  for (vtkIdType r = 0; r < n; ++r)
    {
    for (vtkIdType c = r+1; c < n; ++c)
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

  this->Outputs["correlation graph"] = correlGraphAlg->GetOutput();
}
