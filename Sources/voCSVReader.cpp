
// Visomics includes
#include "voCSVReader.h"
#include "voTableView.h"

// VTK includes
#include <vtkDelimitedTextReader.h>
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

  this->Views["RawData-Table"] = new voTableView();
  this->Views["RawData-Table"]->setInput(this);
}

// --------------------------------------------------------------------------
void voCSVReader::updateInternal()
{
  this->Reader->Update();
  vtkTable* table = this->Reader->GetOutput();

  // Transpose table
  vtkSmartPointer<vtkTable> transpose = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkStringArray> header = vtkSmartPointer<vtkStringArray>::New();
  header->SetName("header");
  header->SetNumberOfTuples(table->GetNumberOfColumns()-1);
  for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
    {
    header->SetValue(c-1, table->GetColumnName(c));
    }
  transpose->AddColumn(header);
  for (vtkIdType r = 0; r < table->GetNumberOfRows(); ++r)
    {
    vtkSmartPointer<vtkStringArray> newcol = vtkSmartPointer<vtkStringArray>::New();
    newcol->SetName(table->GetValue(r, 0).ToString().c_str());
    newcol->SetNumberOfTuples(table->GetNumberOfColumns() - 1);
    for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
      {
      newcol->SetValue(c-1, table->GetValue(r, c).ToString());
      }
    transpose->AddColumn(newcol);
    }

  // Detect numeric columns
  vtkSmartPointer<vtkStringToNumeric> numeric = vtkSmartPointer<vtkStringToNumeric>::New();
  numeric->SetInput(transpose);
  numeric->Update();

  this->Outputs["output"] = numeric->GetOutput();
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
