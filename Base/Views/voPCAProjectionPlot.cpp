
// QT includes
#include <QLayout>
#include <QDebug>
#include <QMap>

// Visomics includes
#include "voDataObject.h"
#include "voPCAProjectionPlot.h"
#include "voUtils.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <voChartXY.h>
#include <vtkContext2D.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>

// --------------------------------------------------------------------------
class voPCAProjectionPlotPrivate
{
public:
  voPCAProjectionPlotPrivate();

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<voChartXY>      Chart;
  vtkPlot*                        Plot;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voPCAProjectionPlotPrivate methods

// --------------------------------------------------------------------------
voPCAProjectionPlotPrivate::voPCAProjectionPlotPrivate()
{
  this->Widget = 0;
  this->Plot = 0;
}

// --------------------------------------------------------------------------
// voPCAProjectionPlot methods

// --------------------------------------------------------------------------
voPCAProjectionPlot::voPCAProjectionPlot(QWidget * newParent):
    Superclass(newParent), d_ptr(new voPCAProjectionPlotPrivate)
{
}

// --------------------------------------------------------------------------
voPCAProjectionPlot::~voPCAProjectionPlot()
{
}

// --------------------------------------------------------------------------
void voPCAProjectionPlot::setupUi(QLayout *layout)
{
  Q_D(voPCAProjectionPlot);

  d->ChartView = vtkSmartPointer<vtkContextView>::New();
  d->Chart = vtkSmartPointer<voChartXY>::New();
  d->Widget = new QVTKWidget();
  d->ChartView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ChartView->GetRenderWindow());
  d->ChartView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  d->ChartView->GetScene()->AddItem(d->Chart);
  d->Plot = d->Chart->AddPlot(vtkChart::POINTS);

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voPCAProjectionPlot::setDataObject(voDataObject *dataObject)
{
  Q_D(voPCAProjectionPlot);

  if (!dataObject)
    {
    qCritical() << "voPCAProjectionPlot - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkTable * table = vtkTable::SafeDownCast(dataObject->data());
  if (!table)
    {
    qCritical() << "voPCAProjectionPlot - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  // Transpose table - this is pretty much unavoidable: vtkPlot expects each dimension
  // to be a column, but the information should be presented to the user with each
  // data point (principle component) in its own column
  vtkSmartPointer<vtkTable> transpose = vtkSmartPointer<vtkTable>::New();
  voUtils::transposeTable(table, transpose, voUtils::Headers);

  // See http://www.colorjack.com/?swatch=A6CEE3
  unsigned char color[3] = {166, 206, 227};

  // TODO Extract only the first two rows of the data table instead of transposing the entire table
  d->Plot->SetInput(transpose, 1, 2);
  d->Plot->SetColor(color[0], color[1], color[2], 255);
  d->Plot->SetWidth(10);

  vtkStringArray* labelArray = vtkStringArray::SafeDownCast(transpose->GetColumn(0));
  if (labelArray)
    {
    std::vector<std::string> labels;
    for(int rid = 0; rid < labelArray->GetNumberOfValues(); ++rid)
      {
      labels.push_back(labelArray->GetValue(rid));
      }
    d->Chart->AddPointLabels(labels);
    }

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(transpose->GetColumnName(1)); // x
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle(transpose->GetColumnName(2)); // y

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
