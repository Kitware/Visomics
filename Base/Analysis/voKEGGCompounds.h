
#ifndef __voKEGGCompounds_h
#define __voKEGGCompounds_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voKEGGCompoundsPrivate;

class voKEGGCompounds : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voKEGGCompounds();
  virtual ~voKEGGCompounds();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();

  virtual bool execute();

protected:
  QScopedPointer<voKEGGCompoundsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voKEGGCompounds);
  Q_DISABLE_COPY(voKEGGCompounds);
};

#endif
