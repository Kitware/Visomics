
// Visomics includes
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

//----------------------------------------------------------------------------
// vtkInternal

//----------------------------------------------------------------------------
class vtkExtendedTable::vtkInternal
{
public:
  vtkInternal();
  
  vtkSmartPointer<vtkTable> ColumnMetaData;
  vtkSmartPointer<vtkTable> RowMetaData;
  vtkSmartPointer<vtkTable> Data;
  
  vtkIdType ColumnMetaDataTypeOfInterest;
  vtkIdType RowMetaDataTypeOfInterest;
};

//----------------------------------------------------------------------------
// vtkInternal methods

//----------------------------------------------------------------------------
vtkExtendedTable::vtkInternal::vtkInternal()
{
  this->ColumnMetaDataTypeOfInterest = -1;
  this->RowMetaDataTypeOfInterest = -1;
}
  
//----------------------------------------------------------------------------
// vtkExtendedTable methods

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkExtendedTable);

//----------------------------------------------------------------------------
vtkExtendedTable::vtkExtendedTable()
{
  this->Internal = new vtkExtendedTable::vtkInternal();
}

//----------------------------------------------------------------------------
vtkExtendedTable::~vtkExtendedTable()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkExtendedTable::PrintSelf(ostream &os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkTable* vtkExtendedTable::GetData()
{
  return this->Internal->Data;
}

//----------------------------------------------------------------------------
vtkTable* vtkExtendedTable::GetColumnMetaData()
{
  return this->Internal->ColumnMetaData;
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetNumberOfColumnMetaDataTypes()const
{
  return this->Internal->ColumnMetaData->GetNumberOfRows();
}

//---------------------------------------------------------------------------- 
vtkAbstractArray* vtkExtendedTable::GetColumnMetaData(vtkIdType id)
{
  return this->Internal->ColumnMetaData->GetColumn(id);
}

//----------------------------------------------------------------------------
//vtkAbstractArray* vtkExtendedTable::GetColumnMetaDataByName(const char* name)
//{
//  return 0;
//}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetColumnMetaDataTypeOfInterest() const
{
  return this->Internal->ColumnMetaDataTypeOfInterest;
}

//----------------------------------------------------------------------------
void vtkExtendedTable::SetColumnMetaDataTypeOfInterest(vtkIdType id)
{
  if (this->Internal->ColumnMetaDataTypeOfInterest == id)
    {
    return;
    }

  if (id < 0 || id >= this->GetNumberOfColumnMetaDataTypes())
    {
    vtkErrorMacro(<< "vtkExtendedTable::SetColumnMetaDataTypeOfInterest - Invalid id:" << id);
    return;
    }
  
  this->Internal->ColumnMetaDataTypeOfInterest = id;
  
  this->Modified();
}

//----------------------------------------------------------------------------
vtkTable* vtkExtendedTable::GetRowMetaData()
{
  return this->Internal->RowMetaData;
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetNumberOfRowMetaDataTypes()const
{
  return this->Internal->RowMetaData->GetNumberOfColumns();
}

//----------------------------------------------------------------------------
vtkVariantArray* vtkExtendedTable::GetRowMetaData(vtkIdType id)
{
  return this->Internal->RowMetaData->GetRow(id);
}

//----------------------------------------------------------------------------
//vtkVariantArray* vtkExtendedTable::GetRowMetaDataByName(const char* name)
//{
//  return 0;
//}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetRowMetaDataTypeOfInterest() const
{
  return this->Internal->ColumnMetaDataTypeOfInterest;
}

//----------------------------------------------------------------------------
void vtkExtendedTable::SetRowMetaDataTypeOfInterest(vtkIdType id)
{
  if (this->Internal->RowMetaDataTypeOfInterest == id)
    {
    return;
    }
    
  if (id < 0 || id >= this->GetNumberOfRowMetaDataTypes())
    {
    vtkErrorMacro(<< "vtkExtendedTable::SetRowMetaDataTypeOfInterest - Invalid id:" << id);
    return;
    }
  
  this->Internal->RowMetaDataTypeOfInterest = id;
  
  this->Modified();
}

