
// Qt includes
#include <QLayout>
#include <QDebug>

// Visomics includes
#include "voTreeGraphView.h"
#include "voDataObject.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkTreeLayoutStrategy.h>
#include <vtkGraph.h>
#include <vtkTree.h>
#include <vtkGraphLayoutView.h>
#include <vtkLookupTable.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
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

  vtkTree * tree = vtkTree::SafeDownCast(dataObject->data());
  if (!tree)
    {
    qCritical() << "voTreeGraphView - Failed to setDataObject - vtkTree data is expected !";
    return;
    }

  d->GraphView->SetRepresentationFromInputConnection(tree->GetProducerPort());
  d->GraphView->ResetCamera();
  d->GraphView->Render();
}
