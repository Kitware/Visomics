
// Visomics includes
#include "voCSVReader.h"
#include "voTableView.h"

// VTK includes
#include <vtkDelimitedTextReader.h>
#include <vtkStringToNumeric.h>
#include <vtkStringArray.h>
#include <vtkStringToNumeric.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
voCSVReader::voCSVReader()
{
  this->Reader = vtkSmartPointer<vtkDelimitedTextReader>::New();
  this->Reader->SetFieldDelimiterCharacters(",");
  this->Reader->SetHaveHeaders(1);
  this->Reader->DetectNumericColumnsOff();
  this->NumericOutput = vtkSmartPointer<vtkStringToNumeric>::New();
}

// --------------------------------------------------------------------------
void voCSVReader::update()
{
  this->Reader->Update();
  // Detect numeric columns
  this->NumericOutput->SetInputConnection(this->Reader->GetOutputPort());
  this->NumericOutput->Update();
}

// --------------------------------------------------------------------------
vtkDataObject* voCSVReader::output()
{
  return this->NumericOutput->GetOutput();
}

// --------------------------------------------------------------------------
void voCSVReader::setFileName(QString file)
{
  this->Reader->SetFileName(file.toAscii().data());
}

// --------------------------------------------------------------------------
QString voCSVReader::fileName()
{
  return this->Reader->GetFileName();
}
