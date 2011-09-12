

#ifndef __voPCAProjectionView_h
#define __voPCAProjectionView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voPCAProjectionViewPrivate;

class voPCAProjectionView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voPCAProjectionView(QWidget * newParent = 0);
  virtual ~voPCAProjectionView();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

protected:
  QScopedPointer<voPCAProjectionViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPCAProjectionView);
  Q_DISABLE_COPY(voPCAProjectionView);
};

#endif
