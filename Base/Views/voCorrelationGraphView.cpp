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
#include <QLayout>
#include <QVariant>

// Visomics includes
#include "voCorrelationGraphView.h"
#include "voDataObject.h"
#include "voInteractorStyleRubberBand2D.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkArcParallelEdgeStrategy.h>
#include <vtkColorTransferFunction.h>
#include <vtkGraph.h>
#include <vtkGraphLayoutView.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkViewTheme.h>

#include <vtkDataSetAttributes.h>

// --------------------------------------------------------------------------
class voCorrelationGraphViewPrivate
{
public:
  voCorrelationGraphViewPrivate();
  vtkSmartPointer<vtkGraphLayoutView> GraphView;
  QVTKWidget*                         Widget;
};

// --------------------------------------------------------------------------
// voCorrelationGraphViewPrivate methods

// --------------------------------------------------------------------------
voCorrelationGraphViewPrivate::voCorrelationGraphViewPrivate()
{
  this->GraphView = 0;
  this->Widget = 0;
}

// --------------------------------------------------------------------------
// voCorrelationGraphView methods

// --------------------------------------------------------------------------
voCorrelationGraphView::voCorrelationGraphView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voCorrelationGraphViewPrivate)
{
}

// --------------------------------------------------------------------------
voCorrelationGraphView::~voCorrelationGraphView()
{
}

// --------------------------------------------------------------------------
void voCorrelationGraphView::setupUi(QLayout *layout)
{
  Q_D(voCorrelationGraphView);

  d->GraphView = vtkSmartPointer<vtkGraphLayoutView>::New();
  d->Widget = new QVTKWidget();
  d->GraphView->SetInteractor(d->Widget->GetInteractor());
  d->GraphView->SetInteractorStyle(vtkSmartPointer<voInteractorStyleRubberBand2D>::New());
  d->Widget->SetRenderWindow(d->GraphView->GetRenderWindow());
  d->GraphView->DisplayHoverTextOn();
  d->GraphView->SetLayoutStrategyToCircular();
  d->GraphView->SetVertexLabelArrayName("label");
  d->GraphView->VertexLabelVisibilityOn();
  d->GraphView->SetEdgeColorArrayName("Correlation");
  d->GraphView->ColorEdgesOn();
  d->GraphView->SetEdgeLabelArrayName("Correlation");
  d->GraphView->EdgeLabelVisibilityOn();

  vtkNew<vtkArcParallelEdgeStrategy> arc;
  arc->SetNumberOfSubdivisions(50);
  d->GraphView->SetEdgeLayoutStrategy(arc.GetPointer());

  vtkRenderedGraphRepresentation* rep =
    vtkRenderedGraphRepresentation::SafeDownCast(d->GraphView->GetRepresentation());
  rep->SetVertexHoverArrayName("name");
  rep->SetEdgeHoverArrayName("Correlation");

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
QString voCorrelationGraphView::hints()const
{
  return QString("<img src=\":/Icons/Bulb.png\">&nbsp;Only correlations more significant than %1 0.5 are displayed.").arg(QChar(177));
}

// --------------------------------------------------------------------------
void voCorrelationGraphView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voCorrelationGraphView);

  vtkGraph * graph = vtkGraph::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!graph)
    {
    qCritical() << "voCorrelationGraphView - Failed to setDataObject - vtkGraph data is expected !";
    return;
    }

  //  vtkNew<vtkLookupTable> lut;
  //  lut->SetHueRange(0.65, 0.65);
  //  lut->SetSaturationRange(1.0, 1.0);
  //  lut->SetValueRange(0.8, 0.8);
  //  lut->SetAlphaRange(0.0, 1.0);
  //  lut->Build();

  double minValue = -1.0;
  if (dataObject.property("min_value").isValid())
    {
    minValue = dataObject.property("min_value").toDouble();
    }
  double maxValue = 1.0;
  if (dataObject.property("max_value").isValid())
    {
    maxValue = dataObject.property("max_value").toDouble();
    }
  double midValue = 0.0;
  if (minValue != -1.0 && maxValue != 1.0)
    {
    midValue = (maxValue-minValue)/2.0;
    }
  double hsvScalars[3] = {minValue, midValue, maxValue};
  double hsvHues[3] = {0.3, 0.15, 0.0}; // green - red
  double hsvSats[3] = {1.0, 0.3, 1.0};
  double hsvValues[3] = {1.0, 0.3, 1.0};

  vtkNew<vtkColorTransferFunction> transferFunction;
  for(int i = 0; i < 3 - 1; ++i)
    {
    transferFunction->AddHSVSegment(hsvScalars[i], hsvHues[i], hsvSats[i], hsvValues[i],
                                    hsvScalars[i+1], hsvHues[i+1], hsvSats[i+1], hsvValues[i+1]);
    }
  transferFunction->Build();

  vtkNew<vtkViewTheme> theme;
  theme->SetBackgroundColor(1.0, 1.0, 1.0);
  theme->SetBackgroundColor2(1.0, 1.0, 1.0);
  theme->SetLineWidth(2);
  theme->SetCellLookupTable(transferFunction.GetPointer());
  theme->GetPointTextProperty()->SetColor(0.0, 0.0, 0.0);
  d->GraphView->ApplyViewTheme(theme.GetPointer());

  d->GraphView->SetRepresentationFromInput(graph);
  d->GraphView->ResetCamera();
  d->GraphView->Render();
}
