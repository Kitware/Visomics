

#ifndef __voPCAProjectionPlot_h
#define __voPCAProjectionPlot_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voPCAProjectionPlotPrivate;

class voPCAProjectionPlot : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voPCAProjectionPlot(QWidget * newParent = 0);
  virtual ~voPCAProjectionPlot();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voPCAProjectionPlotPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPCAProjectionPlot);
  Q_DISABLE_COPY(voPCAProjectionPlot);
};

#endif
