
#ifndef __voANOVAStatistics_h
#define __voANOVAStatistics_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voANOVAStatisticsPrivate;

class voANOVAStatistics : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voANOVAStatistics();
  virtual ~voANOVAStatistics();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();

  virtual bool execute();

protected:
  QScopedPointer<voANOVAStatisticsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voANOVAStatistics);
  Q_DISABLE_COPY(voANOVAStatistics);
};

#endif
