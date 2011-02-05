
// Qt includes
#include <QLayout>
#include <QDebug>

// Visomics includes
#include "voCorrelationGraphView.h"
#include "voDataObject.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkArcParallelEdgeStrategy.h>
#include <vtkGraph.h>
#include <vtkGraphLayoutView.h>
#include <vtkLookupTable.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkViewTheme.h>

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
  d->Widget->SetRenderWindow(d->GraphView->GetRenderWindow());
  d->GraphView->DisplayHoverTextOn();
  d->GraphView->SetLayoutStrategyToCircular();
  d->GraphView->SetVertexLabelArrayName("label");
  d->GraphView->VertexLabelVisibilityOn();
  d->GraphView->SetEdgeColorArrayName("Correlation");
  d->GraphView->ColorEdgesOn();
  d->GraphView->SetEdgeLabelArrayName("Correlation");
  d->GraphView->EdgeLabelVisibilityOn();

  vtkSmartPointer<vtkArcParallelEdgeStrategy> arc =
    vtkSmartPointer<vtkArcParallelEdgeStrategy>::New();
  arc->SetNumberOfSubdivisions(50);
  d->GraphView->SetEdgeLayoutStrategy(arc);

  vtkSmartPointer<vtkViewTheme> theme =
    vtkSmartPointer<vtkViewTheme>::New();
  theme->SetBackgroundColor(1.0, 1.0, 1.0);
  theme->SetBackgroundColor2(1.0, 1.0, 1.0);
  theme->SetLineWidth(2);
  vtkSmartPointer<vtkLookupTable> lut =
    vtkSmartPointer<vtkLookupTable>::New();
  lut->SetHueRange(0.65, 0.65);
  lut->SetSaturationRange(1.0, 1.0);
  lut->SetValueRange(0.8, 0.8);
  lut->SetAlphaRange(0.0, 1.0);
  lut->Build();
  theme->SetCellLookupTable(lut);
  theme->GetPointTextProperty()->SetColor(0.0, 0.0, 0.0);
  d->GraphView->ApplyViewTheme(theme);

  vtkRenderedGraphRepresentation* rep =
    vtkRenderedGraphRepresentation::SafeDownCast(d->GraphView->GetRepresentation());
  rep->SetVertexHoverArrayName("name");
  rep->SetEdgeHoverArrayName("Correlation");

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voCorrelationGraphView::setDataObject(voDataObject* dataObject)
{
  Q_D(voCorrelationGraphView);

  if (!dataObject)
    {
    qCritical() << "voCorrelationGraphView - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkGraph * graph = vtkGraph::SafeDownCast(dataObject->data());
  if (!graph)
    {
    qCritical() << "voCorrelationGraphView - Failed to setDataObject - vtkGraph data is expected !";
    return;
    }

  d->GraphView->SetRepresentationFromInput(graph);
  d->GraphView->Render();
}

