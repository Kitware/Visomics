

#ifndef __voPCAProjectionDynView_h
#define __voPCAProjectionDynView_h

// Visomics includes
#include "voDynView.h"

class voPCAProjectionDynViewPrivate;

class voPCAProjectionDynView : public voDynView
{
  Q_OBJECT
public:
  typedef voDynView Superclass;
  voPCAProjectionDynView(QWidget * newParent = 0);
  virtual ~voPCAProjectionDynView();

protected:
  virtual QString stringify(const voDataObject &dataObject);

protected:
  QScopedPointer<voPCAProjectionDynViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPCAProjectionDynView);
  Q_DISABLE_COPY(voPCAProjectionDynView);
};

#endif
