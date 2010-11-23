
#ifndef __voCSVReader_h
#define __voCSVReader_h

#include "voAnalysis.h"

#include <vtkSmartPointer.h>

class vtkDelimitedTextReader;

class voCSVReader : public voAnalysis
{
public:
  voCSVReader();

  virtual void setFileName(QString fileName);

  virtual QString fileName();

protected:
  virtual void updateInternal();

  vtkSmartPointer<vtkDelimitedTextReader> Reader;
};

#endif
