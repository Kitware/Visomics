

#ifndef __voHierarchicalClusteringHeatMapView_h
#define __voHierarchicalClusteringHeatMapView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voHierarchicalClusteringHeatMapViewPrivate;

class voHierarchicalClusteringHeatMapView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voHierarchicalClusteringHeatMapView(QWidget * newParent = 0);
  virtual ~voHierarchicalClusteringHeatMapView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voHierarchicalClusteringHeatMapViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voHierarchicalClusteringHeatMapView);
  Q_DISABLE_COPY(voHierarchicalClusteringHeatMapView);
};

#endif
