
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
void vtkExtendedTable::Dump() const
{
  std::cout << "ColumnMetaData:";
  if (this->Internal->ColumnMetaData)
    {
    std::cout << std::endl;
    this->Internal->ColumnMetaData->Dump();
    }
  else
    {
    std::cout << "(null)" << std::endl;
    }

  std::cout << "RowMetaData:";
  if (this->Internal->RowMetaData)
    {
    std::cout << std::endl;
    this->Internal->RowMetaData->Dump();
    }
  else
    {
    std::cout << "(null)" << std::endl;
    }

  std::cout << "Data:"<< std::endl;
  this->Dump();
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetTotalNumberOfRows()
{
  int count = this->GetNumberOfColumnMetaDataTypes();
  if (this->GetData())
    {
    count += this->GetNumberOfRows();
    }
  return count;
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetTotalNumberOfColumns()
{
  int count = this->GetNumberOfRowMetaDataTypes();
  if (this->GetData())
    {
    count += this->GetData()->GetNumberOfColumns();
    }
  return count;
}

//----------------------------------------------------------------------------
void vtkExtendedTable::SetData(vtkTable* data)
{
  if (data == 0)
    {
    for(int cid = 0; cid < this->GetNumberOfColumns(); ++cid)
      {
      this->RemoveColumn(cid);
      }
    }
  else
    {
    this->DeepCopy(data);
    }
}

//----------------------------------------------------------------------------
vtkTable* vtkExtendedTable::GetData()
{
  return this;
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetNumberOfColumnMetaDataTypes() const
{
  return this->Internal->ColumnMetaData->GetNumberOfColumns();
}

//---------------------------------------------------------------------------- 
vtkAbstractArray* vtkExtendedTable::GetColumnMetaData(vtkIdType id) const
{
  return this->Internal->ColumnMetaData->GetColumn(id);
}

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
    vtkErrorMacro(<< "vtkExtendedTable::SetColumnMetaDataTypeOfInterest - Invalid id:" << id
                  << " - NumberOfColumnMetaDataTypes:" << this->GetNumberOfColumnMetaDataTypes());
    return;
    }
  
  this->Internal->ColumnMetaDataTypeOfInterest = id;
  
  this->Modified();
}

//----------------------------------------------------------------------------
bool vtkExtendedTable::HasColumnMetaData()const
{
  if (!this->Internal->ColumnMetaData)
    {
    return false;
    }
  return this->Internal->ColumnMetaData->GetNumberOfColumns() > 0;
}

//----------------------------------------------------------------------------
void vtkExtendedTable::SetColumnMetaDataTable(vtkTable* columnMetaData)
{
  if (columnMetaData == this->Internal->ColumnMetaData)
    {
    return;
    }
  this->Internal->ColumnMetaData = columnMetaData;
  this->Modified();
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetNumberOfRowMetaDataTypes() const
{
  return this->Internal->RowMetaData->GetNumberOfColumns();
}

//----------------------------------------------------------------------------
bool vtkExtendedTable::HasRowMetaData() const
{
  if (!this->Internal->RowMetaData)
    {
    return false;
    }
  return this->Internal->RowMetaData->GetNumberOfColumns() > 0;
}

//----------------------------------------------------------------------------
void vtkExtendedTable::SetRowMetaDataTable(vtkTable* rowMetaData)
{
  if (rowMetaData == this->Internal->RowMetaData)
    {
    return;
    }
  this->Internal->RowMetaData = rowMetaData;
  this->Modified();
}

//----------------------------------------------------------------------------
vtkAbstractArray* vtkExtendedTable::GetRowMetaData(vtkIdType id) const
{
  return this->Internal->RowMetaData->GetColumn(id);
}

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
    vtkErrorMacro(<< "vtkExtendedTable::SetRowMetaDataTypeOfInterest - Invalid id:" << id
                  << " - NumberOfRowMetaDataTypes:" << this->GetNumberOfRowMetaDataTypes());
    return;
    }
  
  this->Internal->RowMetaDataTypeOfInterest = id;
  
  this->Modified();
}

//----------------------------------------------------------------------------
vtkAbstractArray* vtkExtendedTable::GetColumnMetaDataOfInterest() const
{
  if (!this->Internal->ColumnMetaData)
    {
    return 0;
    }
  if (this->Internal->ColumnMetaDataTypeOfInterest < 0 ||
      this->Internal->ColumnMetaDataTypeOfInterest >= this->Internal->ColumnMetaData->GetNumberOfColumns())
    {
    return 0;
    }
  return this->Internal->ColumnMetaData->GetColumn(this->Internal->ColumnMetaDataTypeOfInterest);
}

//----------------------------------------------------------------------------
vtkAbstractArray* vtkExtendedTable::GetRowMetaDataOfInterest() const
{
  if (!this->Internal->RowMetaData)
    {
    return 0;
    }
  if (this->Internal->RowMetaDataTypeOfInterest < 0 ||
      this->Internal->RowMetaDataTypeOfInterest >= this->Internal->RowMetaData->GetNumberOfColumns())
    {
    return 0;
    }
  return this->Internal->RowMetaData->GetColumn(this->Internal->RowMetaDataTypeOfInterest);
}
