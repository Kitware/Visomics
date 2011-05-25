
// Qt includes
#include <QLayout>
#include <QDebug>

// Visomics includes
#include "voCorrelationHeatMapView.h"
#include "voDataObject.h"
#include "voUtils.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <vtkChartHistogram2D.h>
#include <vtkColorTransferFunction.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTextProperty.h>

// --------------------------------------------------------------------------
class voCorrelationHeatMapViewPrivate
{
public:
  voCorrelationHeatMapViewPrivate();
  vtkSmartPointer<vtkContextView>       ChartView;
  vtkSmartPointer<vtkChartHistogram2D>  Chart;
  QVTKWidget*                           Widget;
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

  vtkTable * table = vtkTable::SafeDownCast(dataObject->data());
  if (!table)
    {
    qCritical() << "voCorrelationHeatMapView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  vtkStringArray* verticalLabels = vtkStringArray::SafeDownCast(table->GetColumn(0));
  if (!verticalLabels)
    {
    qCritical() << "voCorrelationHeatMapView - Failed to setDataObject - first column of vtkTable data could not be converted to string !";
    return;
    }

  vtkSmartPointer<vtkStringArray> horizontalLabels = vtkSmartPointer<vtkStringArray>::Take(voUtils::tableColumnNames(table, 1));

  vtkNew<vtkDoubleArray> verticalTicks;
  for(double i = table->GetNumberOfRows() - 1; i >= 0; i--)
    {
    verticalTicks->InsertNextValue(i + 0.5);
    }

  vtkNew<vtkDoubleArray> horizontalTicks;
  for(double i = 0.0; i < table->GetNumberOfColumns()-1; i++)
    {
    horizontalTicks->InsertNextValue(i + 0.5);
    }

  // Generate image of the correlation table
  vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  vtkIdType corrMatrixNumberOfCols = table->GetNumberOfColumns();
  vtkIdType corrMatrixNumberOfRows = table->GetNumberOfRows();

  imageData->SetExtent(0, corrMatrixNumberOfCols-2,
                       0, corrMatrixNumberOfRows-1,
                       0,
                       0);
  imageData->SetNumberOfScalarComponents(1);
  imageData->SetScalarTypeToDouble();
  imageData->AllocateScalars();
  imageData->SetOrigin(0.0, 0.0, 0.0);
  imageData->SetSpacing(1.0, 1.0, 1.0);

  double *dPtr = static_cast<double *>(imageData->GetScalarPointer(0, 0, 0));
  //double *dPtr = static_cast<double *>(imageData->GetScalarPointer());
  for (vtkIdType i = 0; i < corrMatrixNumberOfRows; ++i)
    {
    for (vtkIdType j = 1 ; j < corrMatrixNumberOfCols; ++j) // Skip first column (header labels)
      {
      // Flip vertically for table -> image mapping
      dPtr[((corrMatrixNumberOfRows - i -1) * (corrMatrixNumberOfCols - 1)) + (j - 1) ] = table->GetValue(i,j).ToDouble();
      }
    }

  d->Chart->SetInput( imageData );

  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle("");
  d->Chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTickLabels(verticalLabels);
  d->Chart->GetAxis(vtkAxis::LEFT)->SetRange(0.0, static_cast<double>(table->GetNumberOfRows()));
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTickPositions(verticalTicks.GetPointer());

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("");
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTickLabels(horizontalLabels.GetPointer());
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetRange(0.0, static_cast<double>(table->GetNumberOfColumns()-1));
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTickPositions(horizontalTicks.GetPointer());
  d->Chart->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetOrientation(270.0);
  d->Chart->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetJustificationToRight(); // This actually justifies to the left
  d->Chart->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetVerticalJustificationToCentered();

  double hsvScalars[3] = {-1.0, 0.0, 1.0};
  //double hsvHues[3] = {1.0/3.0, 1.0/6.0, 0.0}; // Red - green
  double hsvHues[3] = {0.5, 0.25, 0.0}; // Red - cyan
  double hsvSats[3] = {1.0, 0.3, 1.0};
  double hsvValues[3] = {1.0, 0.3, 1.0};

  vtkSmartPointer<vtkColorTransferFunction> transferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
  for(int i = 0; i < 3 - 1; i++)
    {
    transferFunction->AddHSVSegment(hsvScalars[i], hsvHues[i], hsvSats[i], hsvValues[i],
                                    hsvScalars[i+1], hsvHues[i+1], hsvSats[i+1], hsvValues[i+1]);
    }
  transferFunction->Build();

  d->Chart->SetTransferFunction(transferFunction.GetPointer());

  d->ChartView->Render();
}

