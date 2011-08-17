
#ifndef __voPLSStatistics_h
#define __voPLSStatistics_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voPLSStatisticsPrivate;

class voPLSStatistics : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voPLSStatistics();
  virtual ~voPLSStatistics();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();

  virtual bool execute();

protected:
  QScopedPointer<voPLSStatisticsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPLSStatistics);
  Q_DISABLE_COPY(voPLSStatistics);
};

#endif
