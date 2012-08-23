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
#include "voVolcanoView.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voVolcanoViewPrivate
{
public:
  voVolcanoViewPrivate();

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartXY>     Chart;
  vtkPlot*                        Plot;
  QVTKWidget*                     Widget;
};

// --------------------------------------------------------------------------
// voVolcanoViewPrivate methods

// --------------------------------------------------------------------------
voVolcanoViewPrivate::voVolcanoViewPrivate()
{
  this->Widget = 0;
  this->Plot = 0;
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
  d->Chart = vtkSmartPointer<vtkChartXY>::New();
  d->Widget = new QVTKWidget();
  d->ChartView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ChartView->GetRenderWindow());
  d->ChartView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  d->ChartView->GetScene()->AddItem(d->Chart);
  d->Plot = d->Chart->AddPlot(vtkChart::POINTS);

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voVolcanoView::setDataObjectInternal(const voDataObject& dataObject)
{
   Q_D(voVolcanoView);

  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voVolcanoView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  vtkStringArray* labels = vtkStringArray::SafeDownCast(table->GetColumn(0));
  if (!labels)
    {
    qCritical() << "voVolcanoView - Failed to setDataObject - first column of vtkTable data could not be converted to string !";
    return;
    }

  // See http://www.colorjack.com/?swatch=A6CEE3
  unsigned char color[3] = {166, 206, 227};

  d->Plot->SetInputData(table, 1, 2);
  d->Plot->SetColor(color[0], color[1], color[2], 255);
  d->Plot->SetWidth(10);
  d->Plot->SetIndexedLabels(labels);

  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(table->GetColumnName(1)); // x
  d->Chart->GetAxis(vtkAxis::LEFT)->SetTitle(table->GetColumnName(2)); // y

  // Center the X-axis about 0
  double maxBound = qMax(qAbs(d->Chart->GetAxis(vtkAxis::BOTTOM)->GetMinimum()),
                         qAbs(d->Chart->GetAxis(vtkAxis::BOTTOM)->GetMaximum()));
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
  d->Chart->GetAxis(vtkAxis::BOTTOM)->SetRange(-1 * maxBound, maxBound);

  d->Chart->GetAxis(vtkAxis::LEFT)->SetRange(0.0, 1.0);

  d->ChartView->GetRenderWindow()->SetMultiSamples(4);
  d->ChartView->Render();
}
