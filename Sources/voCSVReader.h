
#ifndef __voCSVReader_h
#define __voCSVReader_h

// Qt includes
#include <QString>

// VTK includes
#include <vtkSmartPointer.h>

class vtkDataObject;
class vtkDelimitedTextReader;
class vtkStringToNumeric;

class voCSVReader
{
public:
  voCSVReader();

  void setFileName(QString fileName);
  QString fileName();

  void update();

  vtkDataObject* output();

  vtkSmartPointer<vtkDelimitedTextReader> Reader;
  vtkSmartPointer<vtkStringToNumeric>     NumericOutput;
};

#endif
