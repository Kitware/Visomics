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
#include "voDataObject.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkDoubleArray.h>
#include <vtkGraph.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToGraph.h>

// --------------------------------------------------------------------------
// voXCorrelPrivate methods

// --------------------------------------------------------------------------
class voXCorrelPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voXCorrel methods

// --------------------------------------------------------------------------
voXCorrel::voXCorrel():
    Superclass(), d_ptr(new voXCorrelPrivate)
{
  Q_D(voXCorrel);
  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
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

  // Parameters
  QString cor_method = this->enumParameter("method");

  // Import data table locally
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qCritical() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> inputDataTable = extendedTable->GetData();

  // Build ArrayData for input to R
  vtkNew<vtkArrayData> RInputArrayData;
    {
    vtkSmartPointer<vtkArray> RInputArray;
    voUtils::tableToArray(inputDataTable.GetPointer(), RInputArray);
    RInputArrayData->AddArray(RInputArray.GetPointer());
    }

  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputConnection(RInputArrayData->GetProducerPort());
  d->RCalc->PutArray("0", "metabData");
  d->RCalc->GetArray("correl","correl");
  d->RCalc->SetRscript(
        QString("correl<-cor(t(metabData), method=\"%1\")").arg(cor_method).toLatin1());
  d->RCalc->Update();

  // Get R output
  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());
  if (!outputArrayData || !outputArrayData->GetArrayByName("correl"))
    {
    qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
    return false;
    }

  // Get analyte names with row labels
  vtkNew<vtkStringArray> analyteNames;
  voUtils::addCounterLabels(extendedTable->GetRowMetaDataOfInterestAsString(),
                            analyteNames.GetPointer(), false);

  // Extract correlation table
  vtkNew<vtkTable> corrTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("correl"), corrTable.GetPointer());
    for (vtkIdType c = 0;c < corrTable->GetNumberOfColumns(); ++c)
      {
      corrTable->GetColumn(c)->SetName(analyteNames->GetValue(c));
      }
    voUtils::insertColumnIntoTable(corrTable.GetPointer(), 0, analyteNames.GetPointer());
    }

  vtkNew<vtkTable> flippedCorrTable;
  voUtils::flipTable(corrTable.GetPointer(), flippedCorrTable.GetPointer(), voUtils::FlipHorizontalAxis, 1, 0);
  this->setOutput("corr",
                  new voTableDataObject("corr", flippedCorrTable.GetPointer(), /* sortable= */ true));
 
  // Find high correlations to put in graph
  vtkNew<vtkTable> sparseCorr;
    {
    vtkNew<vtkStringArray> col1;
    col1->SetName("Column 1");
    vtkNew<vtkStringArray> col2;
    col2->SetName("Column 2");
    vtkNew<vtkDoubleArray> valueArr;
    valueArr->SetName("Correlation");

    vtkIdType corrMatrixNumberOfRows = corrTable->GetNumberOfRows();
    for (vtkIdType r = 0; r < corrMatrixNumberOfRows; ++r)
      {
      for (vtkIdType c = r+1; c < corrMatrixNumberOfRows; ++c)
        {
        double val = corrTable->GetValue(r, c + 1).ToDouble();
        if (qAbs(val) > 0.5)
          {
          col1->InsertNextValue(analyteNames->GetValue(r));
          col2->InsertNextValue(analyteNames->GetValue(c));
          valueArr->InsertNextValue(val);
          }
        }
      }
    sparseCorr->AddColumn(col1.GetPointer());
    sparseCorr->AddColumn(col2.GetPointer());
    sparseCorr->AddColumn(valueArr.GetPointer());
    }

  // Build the graph
  vtkNew<vtkTableToGraph> correlGraphAlg;
  correlGraphAlg->SetInput(sparseCorr.GetPointer());
  correlGraphAlg->AddLinkVertex("Column 1");
  correlGraphAlg->AddLinkVertex("Column 2");
  correlGraphAlg->AddLinkEdge("Column 1", "Column 2");
  correlGraphAlg->Update();

  this->setOutput("correlation_graph", new voDataObject("correlation_graph", correlGraphAlg->GetOutput()));
  return true;
}
