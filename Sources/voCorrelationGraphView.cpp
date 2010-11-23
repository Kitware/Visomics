
// Qt includes
#include <QLayout>

// Visomics includes
#include "voCorrelationGraphView.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkArcParallelEdgeStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkLookupTable.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkViewTheme.h>

// --------------------------------------------------------------------------
voCorrelationGraphView::voCorrelationGraphView()
{
  this->addInput("input");

  this->GraphView = vtkSmartPointer<vtkGraphLayoutView>::New();
  this->Widget = new QVTKWidget();
  this->GraphView->SetInteractor(this->Widget->GetInteractor());
  this->Widget->SetRenderWindow(this->GraphView->GetRenderWindow());
  this->GraphView->DisplayHoverTextOn();
  this->GraphView->SetLayoutStrategyToCircular();
  this->GraphView->SetVertexLabelArrayName("label");
  this->GraphView->VertexLabelVisibilityOn();
  this->GraphView->SetEdgeColorArrayName("Correlation");
  this->GraphView->ColorEdgesOn();
  this->GraphView->SetEdgeLabelArrayName("Correlation");
  this->GraphView->EdgeLabelVisibilityOn();

  vtkSmartPointer<vtkArcParallelEdgeStrategy> arc =
    vtkSmartPointer<vtkArcParallelEdgeStrategy>::New();
  arc->SetNumberOfSubdivisions(50);
  this->GraphView->SetEdgeLayoutStrategy(arc);

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
  this->GraphView->ApplyViewTheme(theme);

  vtkRenderedGraphRepresentation* rep =
    vtkRenderedGraphRepresentation::SafeDownCast(this->GraphView->GetRepresentation());
  rep->SetVertexHoverArrayName("name");
  rep->SetEdgeHoverArrayName("Correlation");
}

// --------------------------------------------------------------------------
void voCorrelationGraphView::updateInternal()
{
  this->GraphView->SetRepresentationFromInput(this->Inputs["input"].data());
  this->GraphView->Render();
}

// --------------------------------------------------------------------------
QWidget* voCorrelationGraphView::widget()
{
  return this->Widget;
}

