
// QT includes
#include <QLayout>
#include <QDebug>
#include <QMap>

// Visomics includes
#include "voPCAProjectionPlot.h"
#include "voDataObject.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkChartXY.h>
#include <vtkContext2D.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

// --------------------------------------------------------------------------
class voPCAProjectionPlotPrivate
{
public:
  voPCAProjectionPlotPrivate();

  QMap<vtkVariant, vtkSmartPointer<vtkTable> > splitTable(vtkTable* t, const char* column);

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartXY>     Chart;
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
QMap<vtkVariant, vtkSmartPointer<vtkTable> > voPCAProjectionPlotPrivate::splitTable(
    vtkTable* t, const char* column)
{
  QMap<vtkVariant, vtkSmartPointer<vtkTable> > m;
  vtkSmartPointer<vtkVariantArray> row =
    vtkSmartPointer<vtkVariantArray>::New();
  for (vtkIdType i = 0; i < t->GetNumberOfRows(); ++i)
    {
    t->GetRow(i, row);
    vtkVariant val = t->GetValueByName(i, column);
    if (m.find(val) == m.end())
      {
      m[val] = vtkSmartPointer<vtkTable>::New();
      for (vtkIdType j = 0; j < t->GetNumberOfColumns(); ++j)
        {
        vtkAbstractArray* a = t->GetColumn(j);
        vtkAbstractArray* b = a->NewInstance();
        b->SetName(a->GetName());
        m[val]->AddColumn(b);
        }
      }
    m[val]->InsertNextRow(row);
    }
  return m;
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
  d->Chart = vtkSmartPointer<vtkChartXY>::New();
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

  QMap<vtkVariant, vtkSmartPointer<vtkTable> > tables = d->splitTable(table, "Y Var");
  QMap<vtkVariant, vtkSmartPointer<vtkTable> >::iterator it, itEnd;
  itEnd = tables.end();
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
	  for (it = tables.begin(); it != itEnd; ++it, ++i)
		{
		vtkPlot* p = d->Chart->AddPlot(vtkChart::POINTS);
		p->SetInput(it.value(), 1, 2);
		p->SetColor(colors[i][0], colors[i][1], colors[i][2], 255);
		p->SetWidth(10);
		}
    }
  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
