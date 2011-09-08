
#ifndef __voFoldChange_h
#define __voFoldChange_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voFoldChangePrivate;

class voFoldChange : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voFoldChange();
  virtual ~voFoldChange();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();
  virtual QString parameterDescription()const;

  virtual bool execute();

protected:
  QScopedPointer<voFoldChangePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voFoldChange);
  Q_DISABLE_COPY(voFoldChange);
};

#endif
