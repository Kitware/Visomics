

#ifndef __voView_h
#define __voView_h

// Qt includes
#include <QList>

// Visomics includes
#include "voAnalysis.h"

class QWidget;

class voView : public voAnalysis
{
public:
  voView() { }

  virtual void updateInternal();

  virtual QWidget* widget() = 0;
};

#endif
