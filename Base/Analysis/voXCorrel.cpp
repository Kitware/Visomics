
// Qt includes
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voXCorrel.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayToTable.h>
#include <vtkDoubleArray.h>
#include <vtkGraph.h>
#include <vtkImageData.h>
#include <vtkNew.h>
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
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voXCorrel::setOutputInformation()
{
  this->addOutputType("corr", "vtkTable",
                      "voHeatMapView", "Correlation Heat Map",
                      "voTableView", "Correlation (Table)");

  this->addOutputType("correlation_graph", "vtkGraph",
                      "voCorrelationGraphView", "Correlation (Graph)");
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

  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qWarning() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::Take(extendedTable->GetDataWithRowHeader());

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
        QString("correl<-cor(t(metabData), method=\"%1\")").arg(cor_method).toLatin1());
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
  vtkSmartPointer<vtkStringArray> header = vtkStringArray::SafeDownCast(table->GetColumn(0));
  if (!header)
    {
    std::cout << "Downcast DID NOT work." << std::endl;
    return 1;
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
  
  vtkTable* assess = XCorProj->GetOutput();
  vtkSmartPointer<vtkTable> corr = vtkSmartPointer<vtkTable>::New();
  corr->AddColumn(header);

  for (vtkIdType c = 0;c < assess->GetNumberOfColumns(); ++c)
    {
    vtkAbstractArray* col = assess->GetColumn(c);
    col->SetName(header->GetValue(c));
    corr->AddColumn(col);
    }

  vtkNew<vtkTable> flippedCorrTable;
  voUtils::flipTable(corr.GetPointer(), flippedCorrTable.GetPointer(), voUtils::FlipHorizontalAxis, 1, 0);
  this->setOutput("corr", new voTableDataObject("corr", flippedCorrTable.GetPointer()));

  // Generate image of the correlation table 
  //  vtkIdType corrMatrixNumberOfCols = corr->GetNumberOfColumns();
    vtkIdType corrMatrixNumberOfRows = corr->GetNumberOfRows();
  //this->setOutput("correlation_heatmap", new voDataObject("correlation_heatmap", imageData));
 
  // Find high correlations to put in graph
  vtkSmartPointer<vtkTable> sparseCorr = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> col1 = vtkSmartPointer<vtkStringArray>::New();
  col1->SetName("Column 1");
  vtkSmartPointer<vtkStringArray> col2 = vtkSmartPointer<vtkStringArray>::New();
  col2->SetName("Column 2");
  vtkSmartPointer<vtkDoubleArray> valueArr = vtkSmartPointer<vtkDoubleArray>::New();
  valueArr->SetName("Correlation");
  for (vtkIdType r = 0; r < corrMatrixNumberOfRows; ++r)
    {
    for (vtkIdType c = r+1; c < corrMatrixNumberOfRows; ++c)
      {
      double val = corr->GetValue(r, c + 1).ToDouble();
      if (val > 0.1)
        {
        col1->InsertNextValue(header->GetValue(r));
        col2->InsertNextValue(header->GetValue(c));
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
