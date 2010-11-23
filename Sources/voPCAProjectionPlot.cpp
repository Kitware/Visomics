
// QT includes
#include <QLayout>
#include <QMap>

// Visomics includes
#include "voPCAProjectionPlot.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkChartXY.h>
#include <vtkContext2D.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

// --------------------------------------------------------------------------
voPCAProjectionPlot::voPCAProjectionPlot()
{  
  this->ChartView = vtkSmartPointer<vtkContextView>::New();
  this->Chart = vtkSmartPointer<vtkChartXY>::New();
  this->Widget = new QVTKWidget();
  this->ChartView->SetInteractor(this->Widget->GetInteractor());
  this->Widget->SetRenderWindow(this->ChartView->GetRenderWindow());
  this->ChartView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  this->ChartView->GetScene()->AddItem(this->Chart);

  this->addInput("input");
}

// --------------------------------------------------------------------------
QMap<vtkVariant, vtkSmartPointer<vtkTable> > voPCAProjectionPlot::splitTable(vtkTable* t, const char* column)
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
void voPCAProjectionPlot::updateInternal()
{
  vtkTable* table = vtkTable::SafeDownCast(this->input().data());

  QMap<vtkVariant, vtkSmartPointer<vtkTable> > tables = this->splitTable(table, "Y Var");
  QMap<vtkVariant, vtkSmartPointer<vtkTable> >::iterator it, itEnd;
  itEnd = tables.end();
  unsigned char colors[10][3] = {{166, 206, 227}, {31, 120, 180}, {178, 223, 13}, {51, 160, 44}, {251, 154, 153}, {227, 26, 28}, {253, 191, 111}, {255, 127, 0}, {202, 178, 214}, {106, 61, 154}};
  int i = 0;
  for (it = tables.begin(); it != itEnd; ++it, ++i)
    {
    vtkPlot* p = this->Chart->AddPlot(vtkChart::POINTS);
    p->SetInput(it.value(), 1, 2);
    p->SetColor(colors[i][0], colors[i][1], colors[i][2], 255);
    p->SetWidth(10);
    }
  this->ChartView->Render();
}

// --------------------------------------------------------------------------
QWidget* voPCAProjectionPlot::widget()
{
  return this->Widget;
}
