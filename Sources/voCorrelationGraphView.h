

#ifndef __voCorrelationGraphView_h
#define __voCorrelationGraphView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voCorrelationGraphViewPrivate;

class voCorrelationGraphView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voCorrelationGraphView(QWidget * newParent = 0);
  virtual ~voCorrelationGraphView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voCorrelationGraphViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voCorrelationGraphView);
  Q_DISABLE_COPY(voCorrelationGraphView);
};

#endif
