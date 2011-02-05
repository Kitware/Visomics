

#ifndef __voTreeGraphView_h
#define __voTreeGraphView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voTreeGraphViewPrivate;

class voTreeGraphView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voTreeGraphView(QWidget * newParent = 0);
  virtual ~voTreeGraphView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voTreeGraphViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTreeGraphView);
  Q_DISABLE_COPY(voTreeGraphView);
};

#endif
