

#ifndef __voHierarchicalClusteringDynView_h
#define __voHierarchicalClusteringDynView_h

// Visomics includes
#include "voDynView.h"

class voHierarchicalClusteringDynViewPrivate;

class voHierarchicalClusteringDynView : public voDynView
{
  Q_OBJECT
public:
  typedef voDynView Superclass;
  voHierarchicalClusteringDynView(QWidget * newParent = 0);
  virtual ~voHierarchicalClusteringDynView();

protected:
  virtual QString stringify(const voDataObject &dataObject);

protected:
  QScopedPointer<voHierarchicalClusteringDynViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voHierarchicalClusteringDynView);
  Q_DISABLE_COPY(voHierarchicalClusteringDynView);
};

#endif
