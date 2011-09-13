/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

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
QString voXCorrel::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Method</b>:</dt>"
                 "<dd>The correlation coefficient used:<br>"
                 "- <i>Pearson's r</i><br>"
                 "- <i>Kendall's %1</i><br>"
                 "- <i>Spearman's %2</i></dd>"
                 "</dl>").arg(QChar(964)).arg(QChar(961));
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
  vtkNew<vtkTableToArray> tab;
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

  vtkNew<vtkArrayData> XCorProjData;
  XCorProjData->AddArray(XCorReturn->GetArrayByName("correl"));

  vtkNew<vtkArrayToTable> XCorProj;
  XCorProj->SetInputConnection(XCorProjData->GetProducerPort());
  XCorProj->Update();  
  
  vtkTable* assess = XCorProj->GetOutput();
  vtkNew<vtkTable> corr;
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
  vtkNew<vtkTable> sparseCorr;
  vtkNew<vtkStringArray> col1;
  col1->SetName("Column 1");
  vtkNew<vtkStringArray> col2;
  col2->SetName("Column 2");
  vtkNew<vtkDoubleArray> valueArr;
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
  sparseCorr->AddColumn(col1.GetPointer());
  sparseCorr->AddColumn(col2.GetPointer());
  sparseCorr->AddColumn(valueArr.GetPointer());

  // Build the graph
  vtkNew<vtkTableToGraph> correlGraphAlg;
  correlGraphAlg->SetInput(sparseCorr.GetPointer());
  correlGraphAlg->AddLinkVertex("Column 1");
  correlGraphAlg->AddLinkVertex("Column 2");
  correlGraphAlg->AddLinkEdge("Column 1", "Column 2");
  correlGraphAlg->Update();

  this->setOutput(
      "correlation_graph", new voDataObject("correlation_graph", correlGraphAlg->GetOutput()));
  return true;
}
