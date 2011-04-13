
// QT includes
#include <QLayout>
#include <QDebug>
#include <QMap>

// Visomics includes
#include "voPCAProjectionPlot.h"
#include "voDataObject.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <voChartXY.h>
#include <vtkContext2D.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
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
  vtkSmartPointer<voChartXY>     Chart;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voPCAProjectionPlotPrivate methods

// --------------------------------------------------------------------------
voPCAProjectionPlotPrivate::voPCAProjectionPlotPrivate()
{
  this->Widget = 0;
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
  vtkSmartPointer<vtkStringArray> header = vtkSmartPointer<vtkStringArray>::New();
  header->SetName("header");
  header->SetNumberOfTuples(table->GetNumberOfColumns()-1);
  for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
    {
    header->SetValue(c-1, table->GetColumnName(c));
    }
  transpose->AddColumn(header);
  for (vtkIdType r = 0; r < table->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkDoubleArray> newcol = vtkSmartPointer<vtkDoubleArray>::New();
    newcol->SetName(table->GetValue(r, 0).ToString().c_str());
    newcol->SetNumberOfTuples(table->GetNumberOfColumns() - 1);
    for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
      {
      newcol->SetValue(c-1, table->GetValue(r, c).ToDouble());
      }
    transpose->AddColumn(newcol);
    }

  unsigned char colors[10][3] =
    {
      {166, 206, 227}, {31, 120, 180}, {178, 223, 13},
      {51, 160, 44}, {251, 154, 153}, {227, 26, 28},
      {253, 191, 111}, {255, 127, 0}, {202, 178, 214}, {106, 61, 154}
    };
  int i = 0;

  vtkPlot* p = d->Chart->GetPlot(0);
	if (!p) 
    {
    p = d->Chart->AddPlot(vtkChart::POINTS);
    }

  p->SetInput(transpose, 1,2);
  p->SetColor(colors[i][0], colors[i][1], colors[i][2], 255);
  p->SetWidth(10);

  // TODO: have voChartXY take a vtkStringArray, rather than std::vector<std::string>
  std::vector<std::string> labels;
  for(int j = 0; j < transpose->GetNumberOfColumns(); j++)
    {
    labels.push_back(transpose->GetValue(j,0).ToString());
    }
  d->Chart->AddPointLabels( labels );

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(transpose->GetColumnName(1)); // x
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle(transpose->GetColumnName(2)); // y

  d->ChartView->GetRenderWindow();
  d->ChartView->Render();

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
