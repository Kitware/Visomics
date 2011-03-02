

#ifndef __voCorrelationHeatMapView_h
#define __voCorrelationHeatMapView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voCorrelationHeatMapViewPrivate;

class voCorrelationHeatMapView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voCorrelationHeatMapView(QWidget * newParent = 0);
  virtual ~voCorrelationHeatMapView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voCorrelationHeatMapViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voCorrelationHeatMapView);
  Q_DISABLE_COPY(voCorrelationHeatMapView);
};

#endif
