
#ifndef __voKMeansClustering_h
#define __voKMeansClustering_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voKMeansClusteringPrivate;

class voKMeansClustering : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voKMeansClustering();
  virtual ~voKMeansClustering();

protected:
  virtual void setInputInformation();
  virtual void setOutputInformation();
  virtual void setParameterInformation();
  virtual QString parameterDescription()const;

  virtual bool execute();

private:
  Q_DECLARE_PRIVATE(voKMeansClustering);
  Q_DISABLE_COPY(voKMeansClustering);
};

#endif
