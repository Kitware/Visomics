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
#include "voInteractorStyleRubberBand2D.h"
#include "voTreeGraphView.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkGraph.h>
#include <vtkGraphLayoutView.h>
#include <vtkLookupTable.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTree.h>
#include <vtkTreeLayoutStrategy.h>
#include <vtkViewTheme.h>

// --------------------------------------------------------------------------
class voTreeGraphViewPrivate
{
public:
  voTreeGraphViewPrivate();
  vtkSmartPointer<vtkGraphLayoutView> GraphView;
  QVTKWidget*                         Widget;
};

// --------------------------------------------------------------------------
// voTreeGraphViewPrivate methods

// --------------------------------------------------------------------------
voTreeGraphViewPrivate::voTreeGraphViewPrivate()
{
  this->GraphView = 0;
  this->Widget = 0;
}

// --------------------------------------------------------------------------
// voTreeGraphView methods

// --------------------------------------------------------------------------
voTreeGraphView::voTreeGraphView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voTreeGraphViewPrivate)
{
}

// --------------------------------------------------------------------------
voTreeGraphView::~voTreeGraphView()
{
}

// --------------------------------------------------------------------------
void voTreeGraphView::setupUi(QLayout *layout)
{
  Q_D(voTreeGraphView);

  d->GraphView = vtkSmartPointer<vtkGraphLayoutView>::New();
  d->Widget = new QVTKWidget();
  d->GraphView->SetInteractor(d->Widget->GetInteractor());
  d->GraphView->SetInteractorStyle(vtkSmartPointer<voInteractorStyleRubberBand2D>::New());
  d->Widget->SetRenderWindow(d->GraphView->GetRenderWindow());
  d->GraphView->DisplayHoverTextOn();

  vtkSmartPointer<vtkTreeLayoutStrategy> treeLayout = vtkSmartPointer<vtkTreeLayoutStrategy>::New();

  d->GraphView->SetLayoutStrategy( treeLayout );
  d->GraphView->VertexLabelVisibilityOn();
  d->GraphView->ColorVerticesOn();
  d->GraphView->SetVertexLabelArrayName("id");

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voTreeGraphView::setDataObject(voDataObject* dataObject)
{
  Q_D(voTreeGraphView);

  if (!dataObject)
    {
    qCritical() << "voTreeGraphView - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkTree * tree = vtkTree::SafeDownCast(dataObject->dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "voTreeGraphView - Failed to setDataObject - vtkTree data is expected !";
    return;
    }

  d->GraphView->SetRepresentationFromInputConnection(tree->GetProducerPort());
  d->GraphView->ResetCamera();
  d->GraphView->Render();
}
