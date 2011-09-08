

#ifndef __voKEGGPathwayView_h
#define __voKEGGPathwayView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voKEGGPathwayViewPrivate;

class voKEGGPathwayView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voKEGGPathwayView(QWidget * newParent = 0);
  virtual ~voKEGGPathwayView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voKEGGPathwayViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voKEGGPathwayView);
  Q_DISABLE_COPY(voKEGGPathwayView);
};

#endif
