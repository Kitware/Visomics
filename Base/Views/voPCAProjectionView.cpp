
// QT includes
#include <QLayout>
#include <QDebug>

// Visomics includes
#include "voDataObject.h"
#include "voPCAProjectionView.h"
#include "voUtils.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voPCAProjectionViewPrivate
{
public:
  voPCAProjectionViewPrivate();

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartXY>     Chart;
  vtkPlot*                        Plot;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voPCAProjectionViewPrivate methods

// --------------------------------------------------------------------------
voPCAProjectionViewPrivate::voPCAProjectionViewPrivate()
{
  this->Widget = 0;
  this->Plot = 0;
}

// --------------------------------------------------------------------------
// voPCAProjectionView methods

// --------------------------------------------------------------------------
voPCAProjectionView::voPCAProjectionView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voPCAProjectionViewPrivate)
{
}

// --------------------------------------------------------------------------
voPCAProjectionView::~voPCAProjectionView()
{
}

// --------------------------------------------------------------------------
void voPCAProjectionView::setupUi(QLayout *layout)
{
  Q_D(voPCAProjectionView);

  d->ChartView = vtkSmartPointer<vtkContextView>::New();
  d->Chart = vtkSmartPointer<vtkChartXY>::New();
  d->Widget = new QVTKWidget();
  d->ChartView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ChartView->GetRenderWindow());
  d->ChartView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  d->ChartView->GetScene()->AddItem(d->Chart);
  d->Plot = d->Chart->AddPlot(vtkChart::POINTS);

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voPCAProjectionView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voPCAProjectionView);

  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voPCAProjectionView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  // Transpose table - this is pretty much unavoidable: vtkPlot expects each dimension
  // to be a column, but the information should be presented to the user with each
  // data point (principle component) in its own column
  vtkNew<vtkTable> transpose;
  voUtils::transposeTable(table, transpose.GetPointer(), voUtils::Headers);

  vtkStringArray* labels = vtkStringArray::SafeDownCast(transpose->GetColumn(0));
  if (!labels)
    {
    qCritical() << "voPCAProjectionView - Failed to setDataObject - first column of vtkTable data could not be converted to string !";
    return;
    }

  // See http://www.colorjack.com/?swatch=A6CEE3
  unsigned char color[3] = {166, 206, 227};

  // TODO Extract only the first two rows of the data table instead of transposing the entire table
  d->Plot->SetInput(transpose.GetPointer(), 1, 2);
  d->Plot->SetColor(color[0], color[1], color[2], 255);
  d->Plot->SetWidth(10);
  d->Plot->SetIndexedLabels(labels);

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(transpose->GetColumnName(1)); // x
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle(transpose->GetColumnName(2)); // y

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
