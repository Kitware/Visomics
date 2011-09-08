
// Qt includes
#include <QDebug>
#include <QList>

// Visomics includes
#include "voDataObject.h"
#include "voPCAProjectionDynView.h"
#include "voUtils.h"

// VTK includes
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voPCAProjectionDynViewPrivate
{
public:
  voPCAProjectionDynViewPrivate();
};

// --------------------------------------------------------------------------
// voPCAProjectionDynViewPrivate methods

// --------------------------------------------------------------------------
voPCAProjectionDynViewPrivate::voPCAProjectionDynViewPrivate()
{
}

// --------------------------------------------------------------------------
// voPCAProjectionDynView methods

// --------------------------------------------------------------------------
voPCAProjectionDynView::voPCAProjectionDynView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voPCAProjectionDynViewPrivate)
{
}

// --------------------------------------------------------------------------
voPCAProjectionDynView::~voPCAProjectionDynView()
{
}

// --------------------------------------------------------------------------
QString voPCAProjectionDynView::stringify(const voDataObject& dataObject)
{
  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voPCAProjectionDynView - Failed to setDataObject - vtkTable data is expected !";
    return QString();
    }
  return voUtils::stringify(this->viewName(), table, QList<vtkIdType>() << 0);
}
