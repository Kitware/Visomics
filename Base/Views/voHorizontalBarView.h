#ifndef __voHorizontalBarView_h
#define __voHorizontalBarView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voHorizontalBarViewPrivate;

class voHorizontalBarView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voHorizontalBarView(QWidget * newParent = 0);
  virtual ~voHorizontalBarView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voHorizontalBarViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voHorizontalBarView);
  Q_DISABLE_COPY(voHorizontalBarView);
};

#endif
