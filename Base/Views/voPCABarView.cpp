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

// Visomics includes
#include "voDataObject.h"
#include "voPCABarView.h"
#include "voUtils.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voPCABarViewPrivate
{
public:
  voPCABarViewPrivate();

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartXY>     Chart;
  vtkPlot*                        Plot;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voPCABarViewPrivate methods

// --------------------------------------------------------------------------
voPCABarViewPrivate::voPCABarViewPrivate()
{
  this->Widget = 0;
  this->Plot = 0;
}

// --------------------------------------------------------------------------
// voPCABarView methods

// --------------------------------------------------------------------------
voPCABarView::voPCABarView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voPCABarViewPrivate)
{
}

// --------------------------------------------------------------------------
voPCABarView::~voPCABarView()
{
}

// --------------------------------------------------------------------------
void voPCABarView::setupUi(QLayout *layout)
{
  Q_D(voPCABarView);

  d->ChartView = vtkSmartPointer<vtkContextView>::New();
  d->Chart = vtkSmartPointer<vtkChartXY>::New();
  d->Widget = new QVTKWidget();
  d->ChartView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ChartView->GetRenderWindow());
  d->ChartView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  d->ChartView->GetScene()->AddItem(d->Chart);
  d->Plot = d->Chart->AddPlot(vtkChart::BAR);
  
  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voPCABarView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voPCABarView);

  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voPCABarView - Failed to setDataObject - vtkTable data is expected !";
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
    qCritical() << "voPCABarView - Failed to setDataObject - first column of vtkTable data could not be converted to string !";
    return;
    }

  vtkNew<vtkStringArray> horizontalLabels;
  vtkNew<vtkDoubleArray> horizontalLocations;
  for(vtkIdType i = 0; i < transpose->GetNumberOfRows(); ++i)
    {
    horizontalLabels->InsertNextValue(QString::number(i+1).toStdString());
    horizontalLocations->InsertNextValue(static_cast<double>(i));
    }

  // See http://www.colorjack.com/?swatch=A6CEE3
  unsigned char color[3] = {166, 206, 227};

  d->Plot->SetInput(transpose.GetPointer(), 1, 2);
  d->Plot->SetColor(color[0], color[1], color[2], 255);
  d->Plot->SetIndexedLabels(labels);
  d->Plot->SetTooltipLabelFormat("%i: %y");

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(transpose->GetColumnName(1)); // x
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTickPositions(horizontalLocations.GetPointer());
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTickLabels(horizontalLabels.GetPointer());
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetRange(-0.5, static_cast<double>(horizontalLocations->GetNumberOfTuples()) - 0.5);
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetGridVisible(false);

  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle(transpose->GetColumnName(2)); // y

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
