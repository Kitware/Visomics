// Qt includes
#include <QDebug>
#include <QLayout>
#include <QMap>

// Visomics includes
#include "voDataObject.h"
#include "voVolcanoView.h"

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
#include <vtkVariantArray.h>

// --------------------------------------------------------------------------
class voVolcanoViewPrivate
{
public:
  voVolcanoViewPrivate();

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<voChartXY>      Chart;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voVolcanoViewPrivate methods

// --------------------------------------------------------------------------
voVolcanoViewPrivate::voVolcanoViewPrivate()
{
  this->Widget = 0;
}

// --------------------------------------------------------------------------
// voVolcanoView methods

// --------------------------------------------------------------------------
voVolcanoView::voVolcanoView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voVolcanoViewPrivate)
{
}

// --------------------------------------------------------------------------
voVolcanoView::~voVolcanoView()
{
}

// --------------------------------------------------------------------------
void voVolcanoView::setupUi(QLayout *layout)
{
  Q_D(voVolcanoView);

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
void voVolcanoView::setDataObject(voDataObject *dataObject)
{
   Q_D(voVolcanoView);

  if (!dataObject)
    {
    qCritical() << "voVolcanoView - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkTable * table = vtkTable::SafeDownCast(dataObject->data());
  if (!table)
    {
    qCritical() << "voVolcanoView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  unsigned char colors[10][3] =
    {
      {166, 206, 227}, {31, 120, 180}, {178, 223, 13},
      {51, 160, 44}, {251, 154, 153}, {227, 26, 28},
      {253, 191, 111}, {255, 127, 0}, {202, 178, 214}, {106, 61, 154}
    };
  vtkPlot* plot = d->Chart->GetPlot(0);
  if (!plot)
    {
    plot = d->Chart->AddPlot(vtkChart::POINTS);
    plot->SetInput(table, 1, 2);
    plot->SetColor(colors[0][0], colors[0][1], colors[0][2], 255);
    plot->SetWidth(10);

    unsigned int rows = table->GetNumberOfRows();
    std::vector<std::string> labels;
    for(unsigned int j = 0; j < rows ; ++j)
      {
      std::string experiment = table->GetValue(j,0).ToString();
      labels.push_back(experiment);
      }

    d->Chart->AddPointLabels(labels);
    //d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("Fold Change: Sample 1 -> Sample 2");
    //d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle("P-Value");
    d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(table->GetColumnName(1)); // x
    d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle(table->GetColumnName(2)); // y

    double maxBound = qMax(qAbs(d->Chart->GetAxis(vtkAxis::BOTTOM)->GetMinimum()),
                           qAbs(d->Chart->GetAxis(vtkAxis::BOTTOM)->GetMaximum()));
    d->Chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
    d->Chart->GetAxis(vtkAxis::BOTTOM)->SetRange(-1 * maxBound, maxBound);
    }
  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
