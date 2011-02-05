
#ifndef __voXCorrel_h
#define __voXCorrel_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voXCorrelPrivate;

class voXCorrel : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voXCorrel();
  virtual ~voXCorrel();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();

  virtual bool execute();

protected:
  QScopedPointer<voXCorrelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voXCorrel);
  Q_DISABLE_COPY(voXCorrel);
};

#endif
