
#ifndef __voTTest_h
#define __voTTest_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voTTestPrivate;

class voTTest : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voTTest();
  virtual ~voTTest();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();

  virtual bool execute();

protected:
  QScopedPointer<voTTestPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTTest);
  Q_DISABLE_COPY(voTTest);
};

#endif
