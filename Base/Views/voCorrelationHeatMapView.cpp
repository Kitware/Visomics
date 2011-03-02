
// Qt includes
#include <QLayout>
#include <QDebug>

// Visomics includes
#include "voCorrelationHeatMapView.h"
#include "voDataObject.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkChartHistogram2D.h>
#include <vtkSmartPointer.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkImageData.h>
#include <vtkColorTransferFunction.h>

// --------------------------------------------------------------------------
class voCorrelationHeatMapViewPrivate
{
public:
  voCorrelationHeatMapViewPrivate();
  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartHistogram2D>  Chart;
  QVTKWidget*                         Widget;
};

// --------------------------------------------------------------------------
// voCorrelationHeatMapViewPrivate methods

// --------------------------------------------------------------------------
voCorrelationHeatMapViewPrivate::voCorrelationHeatMapViewPrivate()
{
  this->ChartView = 0;
  this->Chart= 0;
  this->Widget = 0;
}

// --------------------------------------------------------------------------
// voCorrelationHeatMapView methods

// --------------------------------------------------------------------------
voCorrelationHeatMapView::voCorrelationHeatMapView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voCorrelationHeatMapViewPrivate)
{
}

// --------------------------------------------------------------------------
voCorrelationHeatMapView::~voCorrelationHeatMapView()
{
}

// --------------------------------------------------------------------------
void voCorrelationHeatMapView::setupUi(QLayout *layout)
{
  Q_D(voCorrelationHeatMapView);

  d->ChartView = vtkSmartPointer<vtkContextView>::New();
  d->Chart = vtkSmartPointer<vtkChartHistogram2D>::New();
  d->Widget = new QVTKWidget();
  d->ChartView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ChartView->GetRenderWindow());
  d->ChartView->GetScene()->AddItem( d->Chart );

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voCorrelationHeatMapView::setDataObject(voDataObject* dataObject)
{
  Q_D(voCorrelationHeatMapView);

  if (!dataObject)
    {
    qCritical() << "voCorrelationHeatMapView - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkImageData * imageData = vtkImageData::SafeDownCast(dataObject->data());
  if (!imageData)
    {
    qCritical() << "voCorrelationHeatMapView - Failed to setDataObject - vtkImageData data is expected !";
    return;
    }

  d->Chart->SetInput( imageData );

  vtkSmartPointer<vtkColorTransferFunction> transferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
  transferFunction->AddHSVSegment(0.0, 0.0, 1.0, 1.0,
                                  0.3333, 0.3333, 1.0, 1.0);
  transferFunction->AddHSVSegment(0.3333, 0.3333, 1.0, 1.0,
                                  0.6666, 0.6666, 1.0, 1.0);
  transferFunction->AddHSVSegment(0.6666, 0.6666, 1.0, 1.0,
                                  1.0, 0.2, 1.0, 0.3);
  transferFunction->Build();
  d->Chart->SetTransferFunction(transferFunction.GetPointer());

  d->ChartView->GetRenderWindow();
 
  d->ChartView->Render();
}

