// QT includes
#include <QLayout>
#include <QDebug>
#include <QMap>

// Visomics includes
#include "voPCABarPlot.h"
#include "voDataObject.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContext2D.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTable.h>
//#include <vtkVariantArray.h> //Needed by splitTable
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>

// --------------------------------------------------------------------------
class voPCABarPlotPrivate
{
public:
  voPCABarPlotPrivate();

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartXY>     Chart;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voPCABarPlotPrivate methods

// --------------------------------------------------------------------------
voPCABarPlotPrivate::voPCABarPlotPrivate()
{
  this->Widget = 0;
}

// --------------------------------------------------------------------------
// voPCABarPlot methods

// --------------------------------------------------------------------------
voPCABarPlot::voPCABarPlot(QWidget * newParent):
    Superclass(newParent), d_ptr(new voPCABarPlotPrivate)
{
	//std::cout<<"we are in the bar plot initialization" <<std::endl;
}

// --------------------------------------------------------------------------
voPCABarPlot::~voPCABarPlot()
{

}

// --------------------------------------------------------------------------
void voPCABarPlot::setupUi(QLayout *layout)
{
  Q_D(voPCABarPlot);

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
void voPCABarPlot::setDataObject(voDataObject *dataObject)
{
  Q_D(voPCABarPlot);
  if (!dataObject)
    {
    qCritical() << "voPCABarPlot - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkTable * table = vtkTable::SafeDownCast(dataObject->data());
  if (!table)
    {
    qCritical() << "voPCABarPlot - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  // Transpose table - this is pretty much unavoidable: vtkPlot expects each dimension
  // to be a column, but the information should be presented to the user with each
  // data point (principle component) in its own column
  vtkSmartPointer<vtkTable> transpose = vtkSmartPointer<vtkTable>::New();
  // Note: dont actually need to keep header, but will for consistancy
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
    p = d->Chart->AddPlot(vtkChart::BAR);
    }

  p->SetInput(transpose, 1,2);
  p->SetColor(colors[i][0], colors[i][1], colors[i][2], 255);
  p->SetWidth(10);

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(transpose->GetColumnName(1)); // x
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle(transpose->GetColumnName(2)); // y

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
