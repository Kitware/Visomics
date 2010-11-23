
#ifndef __voPCA_h
#define __voPCA_h

// Qt includes
#include <QList>

// Visomics includes
#include "voAnalysis.h"

class voPCA : public voAnalysis
{
public:
  voPCA();

protected:
  virtual void updateInternal();
};

#endif
