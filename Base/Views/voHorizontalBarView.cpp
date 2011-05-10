// Qt includes
#include <QDebug>
#include <QLayout>
#include <QMap>

// Visomics includes
#include "voDataObject.h"
#include "voHorizontalBarView.h"
#include "voUtils.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <voChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkDoubleArray.h>
#include <vtkPlot.h>
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
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voHorizontalBarViewPrivate methods

// --------------------------------------------------------------------------
voHorizontalBarViewPrivate::voHorizontalBarViewPrivate()
{
  this->Widget = 0;
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
  
  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voHorizontalBarView::setDataObject(voDataObject *dataObject)
{
   Q_D(voHorizontalBarView);
  if (!dataObject)
    {
    qCritical() << "voHorizontalBarView - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkTable * table = vtkTable::SafeDownCast(dataObject->data());
  if (!table)
    {
    qCritical() << "voHorizontalBarView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  //Need a scratch copy, so we can insert a new column for verticalLocations
  vtkSmartPointer<vtkTable> localTable = vtkSmartPointer<vtkTable>::New();
  localTable->DeepCopy(table);

  // verticalLocations is used to set axis tick marks, and as a dimension of the plotted data
  vtkSmartPointer<vtkDoubleArray> verticalLocations = vtkSmartPointer<vtkDoubleArray>::New();
  for(double i = localTable->GetNumberOfRows(); i >= 1.0; i--)
    {
    verticalLocations->InsertNextValue(i);
    }
  verticalLocations->SetName("verticalLocations"); // Will never actually be displayed, but required by vtkPlot
  voUtils::insertColumnIntoTable(localTable, 1, verticalLocations);

  vtkStringArray* verticalLabels = vtkStringArray::SafeDownCast(localTable->GetColumn(0));
  if (!verticalLabels)
    {
    qCritical() << "voHorizontalBarView - Failed to setDataObject - first column of vtkTable data could not be converted to string !";
    return;
    }

  unsigned char colors[10][3] =
    {
      {166, 206, 227}, {31, 120, 180}, {178, 223, 13},
      {51, 160, 44}, {251, 154, 153}, {227, 26, 28},
      {253, 191, 111}, {255, 127, 0}, {202, 178, 214}, {106, 61, 154}
    };

  vtkPlot* p = d->Chart->GetPlot(0);
  if (!p)
    {
    vtkPlot* p = d->Chart->AddPlot(vtkChart::BAR);
    vtkPlotBar* barPlot = vtkPlotBar::SafeDownCast(p);
    barPlot->SetInput(localTable, 1, 2);
    barPlot->SetOrientation(vtkPlotBar::HORIZONTAL);

    barPlot->SetColor(colors[0][0], colors[0][1], colors[0][2], 255);
    barPlot->SetWidth(10);

    d->Chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
    // Default vertical zoom shows all bars at once. If we have many bars, we may want to change this.
    d->Chart->GetAxis(vtkAxis::LEFT)->SetRange(0.0, static_cast<double>(localTable->GetNumberOfRows()) + 1.0);
    d->Chart->GetAxis(vtkAxis::LEFT)->SetTickPositions(verticalLocations);
    d->Chart->GetAxis(vtkAxis::LEFT)->SetTickLabels(verticalLabels);
    d->Chart->GetAxis(vtkAxis::LEFT)->SetGridVisible(false);
    d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle("");

    d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(localTable->GetColumnName(2));

    d->Chart->SetDrawAxesAtOrigin(true);
    }

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
