/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QLayout>
#include <QVariant>

// Visomics includes
#include "voHeatMapView.h"
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
#include <vtkPlotHistogram2D.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTextProperty.h>

// --------------------------------------------------------------------------
class voHeatMapViewPrivate
{
public:
  voHeatMapViewPrivate();
  vtkSmartPointer<vtkContextView>       ChartView;
  vtkSmartPointer<vtkChartHistogram2D>  Chart;
  QVTKWidget*                           Widget;
};

// --------------------------------------------------------------------------
// voHeatMapViewPrivate methods

// --------------------------------------------------------------------------
voHeatMapViewPrivate::voHeatMapViewPrivate()
{
  this->ChartView = 0;
  this->Chart= 0;
  this->Widget = 0;
}

// --------------------------------------------------------------------------
// voHeatMapView methods

// --------------------------------------------------------------------------
voHeatMapView::voHeatMapView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voHeatMapViewPrivate)
{
}

// --------------------------------------------------------------------------
voHeatMapView::~voHeatMapView()
{
}

// --------------------------------------------------------------------------
void voHeatMapView::setupUi(QLayout *layout)
{
  Q_D(voHeatMapView);

  d->ChartView = vtkSmartPointer<vtkContextView>::New();
  d->Chart = vtkSmartPointer<vtkChartHistogram2D>::New();
  d->Widget = new QVTKWidget();
  d->ChartView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ChartView->GetRenderWindow());
  d->ChartView->GetScene()->AddItem( d->Chart );

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voHeatMapView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voHeatMapView);

  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voHeatMapView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  vtkNew<vtkStringArray> verticalLabels;
    {
    // Flip vertical labels
    vtkSmartPointer<vtkStringArray> verticalLabelsRaw = vtkStringArray::SafeDownCast(table->GetColumn(0));
    if (!verticalLabelsRaw)
      {
      qCritical() << "voHeatMapView - Failed to setDataObject - first column of vtkTable data could not be converted to string !";
      return;
      }
    vtkIdType numVertLabels = verticalLabelsRaw->GetNumberOfValues();
    verticalLabels->SetNumberOfValues(numVertLabels);
    for (vtkIdType i = 0; i < numVertLabels; i++)
      {
      verticalLabels->SetValue(i, verticalLabelsRaw->GetValue(numVertLabels-i-1));
      }
    }

  vtkSmartPointer<vtkStringArray> horizontalLabels = vtkSmartPointer<vtkStringArray>::Take(voUtils::tableColumnNames(table, 1));

  vtkNew<vtkDoubleArray> verticalTicks;
  for(double i = 0.0; i < table->GetNumberOfRows(); i++)
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
      double cellValue = table->GetValue(i,j).ToDouble();
      // Flip vertically for table -> image mapping
      dPtr[((corrMatrixNumberOfRows - i -1) * (corrMatrixNumberOfCols - 1)) + (j - 1) ] = cellValue;
      }
    }

  d->Chart->SetInput( imageData );

  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle("");
  d->Chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTickLabels(verticalLabels.GetPointer());
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

  vtkPlotHistogram2D* plotHistogram = vtkPlotHistogram2D::SafeDownCast(d->Chart->GetPlot(0));
  plotHistogram->SetTooltipPrecision(2);
  plotHistogram->SetTooltipNotation(vtkAxis::FIXED_NOTATION);
  plotHistogram->SetTooltipLabelFormat("%j / %i : %v");

  double minValue = -1.0;
  if (dataObject.property("min_value").isValid())
    {
    minValue = dataObject.property("min_value").toDouble();
    }
  double maxValue = 1.0;
  if (dataObject.property("max_value").isValid())
    {
    maxValue = dataObject.property("max_value").toDouble();
    }
  double midValue = 0.0;
  if (minValue != -1.0 && maxValue != 1.0)
    {
    midValue = (maxValue-minValue)/2.0;
    }
  double hsvScalars[3] = {minValue, midValue, maxValue};
  double hsvHues[3] = {0.3, 0.15, 0.0}; // green - red
  double hsvSats[3] = {1.0, 0.3, 1.0};
  double hsvValues[3] = {1.0, 0.3, 1.0};

  vtkNew<vtkColorTransferFunction> transferFunction;
  for(int i = 0; i < 3 - 1; ++i)
    {
    transferFunction->AddHSVSegment(hsvScalars[i], hsvHues[i], hsvSats[i], hsvValues[i],
                                    hsvScalars[i+1], hsvHues[i+1], hsvSats[i+1], hsvValues[i+1]);
    }
  transferFunction->Build();

  d->Chart->SetTransferFunction(transferFunction.GetPointer());

  d->ChartView->Render();
}
