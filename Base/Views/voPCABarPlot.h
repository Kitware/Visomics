

#ifndef __voPCABarPlot_h
#define __voPCABarPlot_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voPCABarPlotPrivate;

class voPCABarPlot : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voPCABarPlot(QWidget * newParent = 0);
  virtual ~voPCABarPlot();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voPCABarPlotPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPCABarPlot);
  Q_DISABLE_COPY(voPCABarPlot);
};

#endif
