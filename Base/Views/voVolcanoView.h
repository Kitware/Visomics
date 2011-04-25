

#ifndef __voVolcanoView_h
#define __voVolcanoView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voVolcanoViewPrivate;

class voVolcanoView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voVolcanoView(QWidget * newParent = 0);
  virtual ~voVolcanoView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voVolcanoViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voVolcanoView);
  Q_DISABLE_COPY(voVolcanoView);
};

#endif
