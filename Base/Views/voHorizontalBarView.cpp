// Qt includes
#include <QDebug>
#include <QLayout>

// Visomics includes
#include "voDataObject.h"
#include "voHorizontalBarView.h"
#include "voUtils.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPlotBar.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voHorizontalBarViewPrivate
{
public:
  voHorizontalBarViewPrivate();

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartXY>     Chart;
  vtkPlotBar*                     BarPlot;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voHorizontalBarViewPrivate methods

// --------------------------------------------------------------------------
voHorizontalBarViewPrivate::voHorizontalBarViewPrivate()
{
  this->Widget = 0;
  this->BarPlot = 0;
}

// --------------------------------------------------------------------------
// voHorizontalBarView methods

// --------------------------------------------------------------------------
voHorizontalBarView::voHorizontalBarView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voHorizontalBarViewPrivate)
{
}

// --------------------------------------------------------------------------
voHorizontalBarView::~voHorizontalBarView()
{
}

// --------------------------------------------------------------------------
void voHorizontalBarView::setupUi(QLayout * layout)
{
  Q_D(voHorizontalBarView);

  d->ChartView = vtkSmartPointer<vtkContextView>::New();
  d->Chart = vtkSmartPointer<vtkChartXY>::New();
  d->Widget = new QVTKWidget();
  d->ChartView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ChartView->GetRenderWindow());
  d->ChartView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  d->ChartView->GetScene()->AddItem(d->Chart);
  d->BarPlot = vtkPlotBar::SafeDownCast(d->Chart->AddPlot(vtkChart::BAR));
  
  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voHorizontalBarView::setDataObjectInternal(voDataObject *dataObject)
{
   Q_D(voHorizontalBarView);

  vtkTable * table = vtkTable::SafeDownCast(dataObject->dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voHorizontalBarView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  //Need a scratch copy, so we can insert a new column for verticalLocations
  vtkNew<vtkTable> localTable;
  localTable->DeepCopy(table);

  // verticalLocations is used to set axis tick marks, and as a dimension of the plotted data
  vtkNew<vtkDoubleArray> verticalLocations;
  for(double i = localTable->GetNumberOfRows(); i >= 1.0; i--)
    {
    verticalLocations->InsertNextValue(i);
    }
  verticalLocations->SetName("verticalLocations"); // Will never actually be displayed, but required by vtkPlot
  voUtils::insertColumnIntoTable(localTable.GetPointer(), 1, verticalLocations.GetPointer());

  vtkStringArray* verticalLabels = vtkStringArray::SafeDownCast(localTable->GetColumn(0));
  if (!verticalLabels)
    {
    qCritical() << "voHorizontalBarView - Failed to setDataObject - first column of vtkTable data could not be converted to string !";
    return;
    }

  // See http://www.colorjack.com/?swatch=A6CEE3
  unsigned char color[3] = {166, 206, 227};

  d->BarPlot->SetInput(localTable.GetPointer(), 1, 2);
  d->BarPlot->SetOrientation(vtkPlotBar::HORIZONTAL);
  d->BarPlot->SetColor(color[0], color[1], color[2], 255);
  d->BarPlot->SetIndexedLabels(verticalLabels);
  d->BarPlot->SetTooltipLabelFormat("%i: %y");

  d->Chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
  // Default vertical zoom shows all bars at once. If we have many bars, we may want to change this.
  d->Chart->GetAxis(vtkAxis::LEFT)->SetRange(0.0, static_cast<double>(localTable->GetNumberOfRows()) + 1.0);
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTickPositions(verticalLocations.GetPointer());
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTickLabels(verticalLabels);
  d->Chart->GetAxis(vtkAxis::LEFT)->SetGridVisible(false);
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle("");

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(localTable->GetColumnName(2));

  d->Chart->SetDrawAxesAtOrigin(true);

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
