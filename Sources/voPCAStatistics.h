
#ifndef __voPCAStatistics_h
#define __voPCAStatistics_h

#include "voAnalysis.h"

#include <QList>

class vtkDescriptiveStatistics;
class vtkPCAStatistics;

class voPCAStatistics : public voAnalysis
{
public:
  voPCAStatistics();

protected:
  virtual void updateInternal();

  vtkSmartPointer<vtkDescriptiveStatistics> Descriptive;
  vtkSmartPointer<vtkPCAStatistics> PCA;
};

#endif
