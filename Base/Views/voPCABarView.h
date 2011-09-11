

#ifndef __voPCABarView_h
#define __voPCABarView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voPCABarViewPrivate;

class voPCABarView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voPCABarView(QWidget * newParent = 0);
  virtual ~voPCABarView();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

protected:
  QScopedPointer<voPCABarViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPCABarView);
  Q_DISABLE_COPY(voPCABarView);
};

#endif
