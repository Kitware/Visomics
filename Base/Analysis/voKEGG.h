
#ifndef __voKEGG_h
#define __voKEGG_h

// Visomics includes
#include "voAnalysis.h"

class voKEGGPrivate;

class voKEGG : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voKEGG();
  virtual ~voKEGG();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();

  virtual bool execute();

private:
  Q_DECLARE_PRIVATE(voKEGG);
  Q_DISABLE_COPY(voKEGG);
};

#endif
