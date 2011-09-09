

#ifndef __voHeatMapView_h
#define __voHeatMapView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voHeatMapViewPrivate;

class voHeatMapView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voHeatMapView(QWidget * newParent = 0);
  virtual ~voHeatMapView();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(voDataObject* dataObject);

protected:
  QScopedPointer<voHeatMapViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voHeatMapView);
  Q_DISABLE_COPY(voHeatMapView);
};

#endif
