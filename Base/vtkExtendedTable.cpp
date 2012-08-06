/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Visomics includes
#include "vtkExtendedTable.h"
#include "voUtils.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
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

  vtkSmartPointer<vtkStringArray> ColumnMetaDataLabels;
  vtkSmartPointer<vtkStringArray> RowMetaDataLabels;
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
void vtkExtendedTable::Dump()
{
  std::cout << "ColumnMetaDataLabels:";
  if (this->Internal->ColumnMetaDataLabels)
    {
    std::cout << std::endl;
    for (vtkIdType i = 0; i < this->Internal->ColumnMetaDataLabels->GetNumberOfValues(); i++)
      {
      std::cout << this->Internal->ColumnMetaDataLabels->GetValue(i) << std::endl;
      }
    }
  else
    {
    std::cout << "(null)" << std::endl;
    }
  std::cout << std::endl;

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
  std::cout << std::endl;

  std::cout << "RowMetaDataLabels:";
  if (this->Internal->RowMetaDataLabels)
    {
    std::cout << std::endl;
    for (vtkIdType i = 0; i < this->Internal->RowMetaDataLabels->GetNumberOfValues(); i++)
      {
      std::cout << this->Internal->RowMetaDataLabels->GetValue(i) << std::endl;
      }
    }
  else
    {
    std::cout << "(null)" << std::endl;
    }
  std::cout << std::endl;

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
  std::cout << std::endl;

  std::cout << "Data:"<< std::endl;
  Superclass::Dump();
  std::cout << std::endl;
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
vtkTable* vtkExtendedTable::GetDataWithRowHeader()
{
  vtkTable * dataWithRowHeader = vtkTable::New();
  dataWithRowHeader->ShallowCopy(this);
  voUtils::insertColumnIntoTable(dataWithRowHeader, 0, this->GetRowMetaDataOfInterestAsString());
  return dataWithRowHeader;
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetNumberOfColumnMetaDataTypes() const
{
  return this->Internal->ColumnMetaData->GetNumberOfColumns();
}

//----------------------------------------------------------------------------
vtkAbstractArray* vtkExtendedTable::GetColumnMetaData(vtkIdType id) const
{
  if (!this->Internal->ColumnMetaData)
    {
    return 0;
    }
  if (id < 0 ||
      id >= this->Internal->ColumnMetaData->GetNumberOfColumns())
    {
    return 0;
    }
  return this->Internal->ColumnMetaData->GetColumn(id);
}

//----------------------------------------------------------------------------
vtkStringArray* vtkExtendedTable::GetColumnMetaDataAsString(vtkIdType id) const
{
  return vtkStringArray::SafeDownCast(this->GetColumnMetaData(id));
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
vtkStringArray* vtkExtendedTable::GetColumnMetaDataOfInterestAsString() const
{
  return vtkStringArray::SafeDownCast(this->GetColumnMetaDataOfInterest());
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
void vtkExtendedTable::SetColumnMetaDataLabels(vtkStringArray* columnMetaDataLabels)
{
  if (columnMetaDataLabels == this->Internal->ColumnMetaDataLabels)
    {
    return;
    }
  this->Internal->ColumnMetaDataLabels = columnMetaDataLabels;
  this->Modified();
}

//----------------------------------------------------------------------------
vtkStringArray* vtkExtendedTable::GetColumnMetaDataLabels() const
{
  return this->Internal->ColumnMetaDataLabels;
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
  if (!this->Internal->RowMetaData)
    {
    return 0;
    }
  if (id < 0 ||
      id >= this->Internal->RowMetaData->GetNumberOfColumns())
    {
    return 0;
    }
  return this->Internal->RowMetaData->GetColumn(id);
}

//----------------------------------------------------------------------------
vtkStringArray* vtkExtendedTable::GetRowMetaDataAsString(vtkIdType id) const
{
  return vtkStringArray::SafeDownCast(this->GetRowMetaData(id));
}

//----------------------------------------------------------------------------
vtkIdType vtkExtendedTable::GetRowMetaDataTypeOfInterest() const
{
  return this->Internal->RowMetaDataTypeOfInterest;
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

//----------------------------------------------------------------------------
vtkStringArray* vtkExtendedTable::GetRowMetaDataOfInterestAsString() const
{
  return vtkStringArray::SafeDownCast(this->GetRowMetaDataOfInterest());
}

//----------------------------------------------------------------------------
void vtkExtendedTable::SetRowMetaDataLabels(vtkStringArray* rowMetaDataLabels)
{
  if (rowMetaDataLabels == this->Internal->RowMetaDataLabels)
    {
    return;
    }
  this->Internal->RowMetaDataLabels = rowMetaDataLabels;
  this->Modified();
}

//----------------------------------------------------------------------------
vtkStringArray* vtkExtendedTable::GetRowMetaDataLabels() const
{
  return this->Internal->RowMetaDataLabels;
}
