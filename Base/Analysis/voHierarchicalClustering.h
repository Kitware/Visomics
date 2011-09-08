
#ifndef __voHierarchicalClustering_h
#define __voHierarchicalClustering_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voHierarchicalClustering : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voHierarchicalClustering();
  virtual ~voHierarchicalClustering();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();
  virtual QString parameterDescription()const;

  virtual bool execute();

private:
  Q_DISABLE_COPY(voHierarchicalClustering);
};

#endif
