
// Qt includes
#include <QDebug>
#include <QLayout>

// Visomics includes
#include "voDataObject.h"
#include "voInteractorStyleRubberBand2D.h"
#include "voKEGGPathwayView.h"

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
class voKEGGPathwayViewPrivate
{
public:
  voKEGGPathwayViewPrivate();
  vtkSmartPointer<vtkGraphLayoutView> GraphView;
  QVTKWidget*                         Widget;
};

// --------------------------------------------------------------------------
// voKEGGPathwayViewPrivate methods

// --------------------------------------------------------------------------
voKEGGPathwayViewPrivate::voKEGGPathwayViewPrivate()
{
  this->GraphView = 0;
  this->Widget = 0;
}

// --------------------------------------------------------------------------
// voKEGGPathwayView methods

// --------------------------------------------------------------------------
voKEGGPathwayView::voKEGGPathwayView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voKEGGPathwayViewPrivate)
{
}

// --------------------------------------------------------------------------
voKEGGPathwayView::~voKEGGPathwayView()
{
}

// --------------------------------------------------------------------------
void voKEGGPathwayView::setupUi(QLayout *layout)
{
  Q_D(voKEGGPathwayView);

  d->GraphView = vtkSmartPointer<vtkGraphLayoutView>::New();
  d->Widget = new QVTKWidget();
  d->GraphView->SetInteractor(d->Widget->GetInteractor());
  d->GraphView->SetInteractorStyle(vtkSmartPointer<voInteractorStyleRubberBand2D>::New());
  d->Widget->SetRenderWindow(d->GraphView->GetRenderWindow());

  d->GraphView->SetLayoutStrategyToSimple2D();
  d->GraphView->SetVertexLabelArrayName("PedigreeIds");
  d->GraphView->VertexLabelVisibilityOn();


/*  d->GraphView->DisplayHoverTextOn();
  d->GraphView->SetLayoutStrategyToCircular();
  d->GraphView->SetVertexLabelArrayName("label");
  d->GraphView->VertexLabelVisibilityOn();
  d->GraphView->SetEdgeColorArrayName("Correlation");
  d->GraphView->ColorEdgesOn();
  d->GraphView->SetEdgeLabelArrayName("Correlation");
  d->GraphView->EdgeLabelVisibilityOn();
*/
  /*
  vtkSmartPointer<vtkArcParallelEdgeStrategy> arc =
    vtkSmartPointer<vtkArcParallelEdgeStrategy>::New();
  arc->SetNumberOfSubdivisions(50);
  d->GraphView->SetEdgeLayoutStrategy(arc);
  */
/*
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
*/
  /*
  vtkRenderedGraphRepresentation* rep =
    vtkRenderedGraphRepresentation::SafeDownCast(d->GraphView->GetRepresentation());
  rep->SetVertexHoverArrayName("name");
  rep->SetEdgeHoverArrayName("Correlation");
*/
  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voKEGGPathwayView::setDataObjectInternal(voDataObject* dataObject)
{
  Q_D(voKEGGPathwayView);

  vtkGraph * graph = vtkGraph::SafeDownCast(dataObject->dataAsVTKDataObject());
  if (!graph)
    {
    qCritical() << "voKEGGPathwayView - Failed to setDataObject - vtkGraph data is expected !";
    return;
    }

  d->GraphView->SetRepresentationFromInput(graph);
  d->GraphView->ResetCamera();
  d->GraphView->Render();
}
