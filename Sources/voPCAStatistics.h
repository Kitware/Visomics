
#ifndef __voPCAStatistics_h
#define __voPCAStatistics_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voPCAStatisticsPrivate;

class voPCAStatistics : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voPCAStatistics();
  virtual ~voPCAStatistics();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();

  virtual bool execute();

protected:
  QScopedPointer<voPCAStatisticsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPCAStatistics);
  Q_DISABLE_COPY(voPCAStatistics);
};

#endif
