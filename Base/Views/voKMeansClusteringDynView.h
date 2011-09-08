

#ifndef __voKMeansClusteringDynView_h
#define __voKMeansClusteringDynView_h

// Visomics includes
#include "voDynView.h"

class voKMeansClusteringDynViewPrivate;

class voKMeansClusteringDynView : public voDynView
{
  Q_OBJECT
public:
  typedef voDynView Superclass;
  voKMeansClusteringDynView(QWidget * newParent = 0);
  virtual ~voKMeansClusteringDynView();

protected:
  virtual QString stringify(const voDataObject &dataObject);

protected:
  QScopedPointer<voKMeansClusteringDynViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voKMeansClusteringDynView);
  Q_DISABLE_COPY(voKMeansClusteringDynView);
};

#endif
