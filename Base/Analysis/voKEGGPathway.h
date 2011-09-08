
#ifndef __voKEGGPathway_h
#define __voKEGGPathway_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voKEGGPathwayPrivate;

class voKEGGPathway : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voKEGGPathway();
  virtual ~voKEGGPathway();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();
  virtual QString parameterDescription()const;

  virtual bool execute();

protected:
  QScopedPointer<voKEGGPathwayPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voKEGGPathway);
  Q_DISABLE_COPY(voKEGGPathway);
};

#endif
