
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voXCorrel.h"
#include "voTableDataObject.h"

// VTK includes
#include <vtkArrayToTable.h>
#include <vtkDoubleArray.h>
#include <vtkGraph.h>
#include <vtkImageData.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkTableToGraph.h>

// --------------------------------------------------------------------------
// voXCorrelPrivate methods

// --------------------------------------------------------------------------
class voXCorrelPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> XCor;
};

// --------------------------------------------------------------------------
// voXCorrel methods

// --------------------------------------------------------------------------
voXCorrel::voXCorrel():
    Superclass(), d_ptr(new voXCorrelPrivate)
{
  Q_D(voXCorrel);
  d->XCor = vtkSmartPointer<vtkRCalculatorFilter>::New();
}

// --------------------------------------------------------------------------
voXCorrel::~voXCorrel()
{
}

// --------------------------------------------------------------------------
void voXCorrel::setInputInformation()
{
  this->addInputType("input", "vtkTable");
}

// --------------------------------------------------------------------------
void voXCorrel::setOutputInformation()
{
  this->addOutputType("corr", "vtkTable",
                      "", "",
                      "voTableView", "Table (Correlation)");
  
  this->addOutputType("correlation_graph", "vtkGraph",
                      "voCorrelationGraphView", "Correlation Graph");

  this->addOutputType("correlation_heatmap", "vtkImageData",
                      "voCorrelationHeatMapView", "Correlation Heat Map");   
}

// --------------------------------------------------------------------------
void voXCorrel::setParameterInformation()
{
  QList<QtProperty*> cor_parameters;

  // Cor / Method
  QStringList cor_methods;
  cor_methods << "pearson" << "kendall" << "spearman";
  cor_parameters << this->addEnumParameter("method", tr("Method"), cor_methods);

  this->addParameterGroup("Correlation parameters", cor_parameters);
}

// --------------------------------------------------------------------------
bool voXCorrel::execute()
{
  Q_D(voXCorrel);

  vtkTable* table =  vtkTable::SafeDownCast(this->input()->data());
  if (!table)
    {
    qWarning() << "Input is Null";
    return false;
    }

  // Parameters
  QString cor_method = this->enumParameter("method");

  //table->Print(std::cout);
  vtkSmartPointer<vtkTableToArray> tab = vtkSmartPointer<vtkTableToArray>::New();
  tab->SetInput(table);
  //table->Print(std::cout);

  for (int ctr=1; ctr<table->GetNumberOfColumns(); ctr++)
    {
    tab->AddColumn(table->GetColumnName(ctr));
    }
  tab->Update();
  
 // tab->GetOutput()->Print(std::cout);

  d->XCor->SetRoutput(0);
  d->XCor->SetInputConnection(tab->GetOutputPort());
  d->XCor->PutArray("0", "metabData");
  d->XCor->SetRscript(
        QString("correl<-cor(metabData, method=\"%1\")").arg(cor_method).toLatin1());
  d->XCor->GetArray("correl","correl");
 
  // Do Cross Correlation
  d->XCor->Update();

  vtkArrayData *XCorReturn = vtkArrayData::SafeDownCast(d->XCor->GetOutput());
  if (!XCorReturn)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
    }

  // Set up headers for the rows.
  vtkIdType cols = table->GetNumberOfColumns() - 1;
  vtkIdType rows = table->GetNumberOfRows();
  vtkSmartPointer<vtkStringArray> header = vtkSmartPointer<vtkStringArray>::New();
  header->SetName("header");
  header->SetNumberOfTuples(cols);
  for (vtkIdType i = 0; i < cols; ++i)
    {
    header->SetValue(i, table->GetColumnName(i + 1));
    }

  // Extract rotated coordinates
  if (!XCorReturn->GetArrayByName("correl"))
    {
    // We should pop up an error message modal window here and return.  For now, cerr will do
    std::cerr << "R did not return a valid reponse probably due to memory issues.  Cannot display cross correlation result." << std::endl;
    return false;
    }

  vtkSmartPointer<vtkArrayData> XCorProjData = vtkSmartPointer<vtkArrayData>::New();
  XCorProjData->AddArray(XCorReturn->GetArrayByName("correl"));

  vtkSmartPointer<vtkArrayToTable> XCorProj = vtkSmartPointer<vtkArrayToTable>::New();
  XCorProj->SetInputConnection(XCorProjData->GetProducerPort());
  XCorProj->Update();  
  
  vtkTable* assess = vtkTable::SafeDownCast(XCorProj->GetOutput());
  vtkSmartPointer<vtkTable> corr = vtkSmartPointer<vtkTable>::New();
  corr->AddColumn(header);

  // assess->Print(std::cout);
  for (vtkIdType c = 0;c < cols; ++c)  //c < assess->GetNumberOfColumns()
    {
    vtkAbstractArray* col = assess->GetColumn(c);
    col->SetName(table->GetColumnName(c + 1));
    //corr->Print(std::cout);
    //col->Print(std::cout);
    corr->AddColumn(col);
    }
  this->setOutput("corr", new voTableDataObject("corr", corr));

  // Generate image of the correlation table 
  vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  vtkIdType corrMatrixNumberOfCols = corr->GetNumberOfColumns();
  vtkIdType corrMatrixNumberOfRows = corr->GetNumberOfRows();

  //std::cout << "Number Of Cols =\t"  << corrMatrixNumberOfCols << std::endl;
  //std::cout << "Number Of Rows =\t"  << corrMatrixNumberOfRows << std::endl;

  imageData->SetExtent(0, corrMatrixNumberOfRows-1, 
                       0, corrMatrixNumberOfRows-1,
                       0,
                       0);
  imageData->SetNumberOfScalarComponents(1);
  imageData->SetScalarTypeToDouble();
  imageData->AllocateScalars();
  imageData->SetOrigin(0.0, 0.0, 0.0);
  imageData->SetSpacing(1.0, 1.0, 1.0);

  double *dPtr = static_cast<double *>(imageData->GetScalarPointer(0, 0, 0));
  for (vtkIdType i = 0; i < corrMatrixNumberOfRows; ++i)
    {
    for (vtkIdType j = 1 ; j < corrMatrixNumberOfRows+1 ; ++j)
      {
      dPtr[i * (corrMatrixNumberOfRows ) + j - 1 ] = corr->GetValue(i,j).ToDouble();
      //std::cout << corr->GetValue(i,j).ToDouble() << " ";
      }
      //std::cout << "\n" << std::endl;
    }
  this->setOutput("correlation_heatmap", new voDataObject("correlation_heatmap", imageData));
 
  // Find high correlations to put in graph
  vtkSmartPointer<vtkTable> sparseCorr = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> col1 = vtkSmartPointer<vtkStringArray>::New();
  col1->SetName("Column 1");
  vtkSmartPointer<vtkStringArray> col2 = vtkSmartPointer<vtkStringArray>::New();
  col2->SetName("Column 2");
  vtkSmartPointer<vtkDoubleArray> valueArr = vtkSmartPointer<vtkDoubleArray>::New();
  valueArr->SetName("Correlation");
  for (vtkIdType r = 0; r < rows; ++r)
    {
    for (vtkIdType c = r+1; c < rows; ++c)
      {
      double val = corr->GetValue(r, c + 1).ToDouble();
      if (val > 0.1)
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
      "correlation_graph", new voDataObject("correlation_graph", correlGraphAlg->GetOutput()));
  return true;
}