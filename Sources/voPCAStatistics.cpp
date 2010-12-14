
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
#include <vtkPCAStatistics.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkTableToGraph.h>


// --------------------------------------------------------------------------
class voPCAStatisticsPrivate
{
public:

  vtkSmartPointer<vtkDescriptiveStatistics> Descriptive;
  vtkSmartPointer<vtkPCAStatistics> PCA;
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

  d->PCA = vtkSmartPointer<vtkPCAStatistics>::New();
  d->PCA->SetLearnOption(true);
  d->PCA->SetDeriveOption(true);
  d->PCA->SetAssessOption(true);
  d->PCA->SetBasisSchemeByName("FixedBasisEnergy");
  d->PCA->SetFixedBasisEnergy(0.95);

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
  this->addOutputType("x", "vtkTable",
                      "voPCAProjectionPlot", "Projection Plot",
                      "voTableView", "Table (Projection Plot)");

  this->addOutputType("x", "vtkTable",
                      "", "",
                      "voTableView", "Table (Coordinates)");

  this->addOutputType("rot", "vtkTable",
                      "",  "",
                      "voTableView",  "Table (Rotation)");

  this->addOutputType("sdev", "vtkTable",
                      "", "",
                      "voTableView", "Table (Std. Deviation)");

  this->addOutputType("corr", "vtkTable",
                      "", "",
                      "voTableView", "Table (Correlation)");

  this->addOutputType("correlation graph", "vtkGraph",
                      "voCorrelationGraphView", "Correlation Graph");
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

  // Add request to process all columns
  d->PCA->ResetRequests();
  d->PCA->ResetAllColumnStates();
  for (int i = 1; i < table->GetNumberOfColumns(); ++i)
    {
    d->PCA->SetColumnStatus(table->GetColumnName(i), 1);
    }
  d->PCA->RequestSelectedColumns();

  // Do PCA
  d->PCA->SetInput(table);
  d->PCA->Update();

  vtkTable* assess = vtkTable::SafeDownCast(d->PCA->GetOutputDataObject(0));
  vtkMultiBlockDataSet* learn = vtkMultiBlockDataSet::SafeDownCast(d->PCA->GetOutputDataObject(1));

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
  for (int i = 1; i < table->GetNumberOfColumns(); ++i)
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

  // Extract rotated coordinates
  vtkSmartPointer<vtkTable> xtab = vtkSmartPointer<vtkTable>::New();
  xtab->AddColumn(assess->GetColumn(0));
  for (vtkIdType c = table->GetNumberOfColumns(); c < assess->GetNumberOfColumns(); ++c)
    {
    vtkAbstractArray* col = assess->GetColumn(c);
    col->SetName(vtkVariant(c-table->GetNumberOfColumns()).ToString());
    xtab->AddColumn(col);
    }
  this->setOutput("x", new voTableDataObject("x", xtab));

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
  this->setOutput("rot", new voTableDataObject("rot", rot));

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
  this->setOutput("sdev", new voTableDataObject("sdev", sdev));

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
  this->setOutput("corr", new voTableDataObject("corr", corr));

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

  this->setOutput(
      "correlation graph", new voDataObject("correlation graph", correlGraphAlg->GetOutput()));

  return true;
}
