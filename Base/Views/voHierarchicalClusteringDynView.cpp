
// Qt includes
#include <QDebug>
#include <QList>

// Visomics includes
#include "voDataObject.h"
#include "voHierarchicalClusteringDynView.h"
#include "voUtils.h"

// VTK includes
#include <vtkTree.h>
#include <vtkGraph.h>
#include <vtkTreeLevelsFilter.h>
#include <vtkDataSetAttributes.h>
#include <vtkAbstractArray.h>

// --------------------------------------------------------------------------
class voHierarchicalClusteringDynViewPrivate
{
public:
  voHierarchicalClusteringDynViewPrivate();
};

// --------------------------------------------------------------------------
// voHierarchicalClusteringDynViewPrivate methods

// --------------------------------------------------------------------------
voHierarchicalClusteringDynViewPrivate::voHierarchicalClusteringDynViewPrivate()
{
}

// --------------------------------------------------------------------------
// voHierarchicalClusteringDynView methods

// --------------------------------------------------------------------------
voHierarchicalClusteringDynView::voHierarchicalClusteringDynView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voHierarchicalClusteringDynViewPrivate)
{
}

// --------------------------------------------------------------------------
voHierarchicalClusteringDynView::~voHierarchicalClusteringDynView()
{
}

// --------------------------------------------------------------------------
QString voHierarchicalClusteringDynView::stringify(const voDataObject& dataObject)
{
  vtkTree * tree = vtkTree::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "voHierarchicalClusteringDynView - Failed to setDataObject - vtkTree data is expected !";
    return QString();
    }


  return voUtils::stringify(this->viewName(), tree);

}
