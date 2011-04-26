
// Qt includes
#include <QtGlobal>
#include <QStringList>
#include <QRegExp>

// Visomics includes
#include "voUtils.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

#include <QDebug>

namespace
{
//----------------------------------------------------------------------------
template<typename ArrayType, typename ValueType>
bool transposeColumn(vtkTable* srcTable, vtkTable* destTable, int columnId, bool useVariant = false)
{
  if (!srcTable || !destTable)
    {
    return false;
    }
  if (columnId < 0 || columnId >= srcTable->GetNumberOfColumns())
    {
    return false;
    }
  vtkAbstractArray * column = srcTable->GetColumn(columnId);
  ArrayType * typeColumn = ArrayType::SafeDownCast(column);
  if (!typeColumn && !useVariant)
    {
    return false;
    }

  for (int rid = 0; rid < column->GetNumberOfTuples() * column->GetNumberOfComponents(); ++rid)
    {
    vtkSmartPointer<ArrayType> transposedColumn;
    if (columnId == 0)
      {
      transposedColumn = vtkSmartPointer<ArrayType>::New();
      transposedColumn->SetNumberOfValues(srcTable->GetNumberOfColumns());
      destTable->AddColumn(transposedColumn);
      }
    else
      {
      transposedColumn = ArrayType::SafeDownCast(destTable->GetColumn(rid));
      }
    Q_ASSERT(transposedColumn);
    if (!useVariant)
      {
      ValueType value = typeColumn->GetValue(rid);
      transposedColumn->SetValue(columnId, value);
      }
    else
      {
      vtkVariant value = column->GetVariantValue(rid);
      transposedColumn->SetVariantValue(columnId, value);
      }
    }
  return true;
}
}

//----------------------------------------------------------------------------
bool voUtils::transposeTable(vtkTable* srcTable, vtkTable* destTable)
{
  if (!srcTable)
    {
    return false;
    }

  if (!destTable)
    {
    return false;
    }

  if (srcTable->GetNumberOfColumns() == 0)
    {
    return true;
    }

  bool useVariant = false;
  vtkAbstractArray * firstColumn = srcTable->GetColumn(0);
  for (int cid = 1; cid < srcTable->GetNumberOfColumns(); ++cid)
    {
    if (qstrcmp(firstColumn->GetClassName(), srcTable->GetColumn(cid)->GetClassName()) != 0)
      {
      useVariant = true;
      break;
      }
    }

  for(int cid = 0; cid < srcTable->GetNumberOfColumns(); ++cid)
    {
    if (!useVariant)
      {
      if (!transposeColumn<vtkDoubleArray, double>(srcTable, destTable, cid))
        {
        if (!transposeColumn<vtkIntArray, int>(srcTable, destTable, cid))
          {
          if (!transposeColumn<vtkStringArray, vtkStdString>(srcTable, destTable, cid))
            {
            if (!transposeColumn<vtkVariantArray, vtkVariant>(srcTable, destTable, cid))
              {
              return false;
              }
            }
          }
        }
      }
    else
      {
      if (!transposeColumn<vtkVariantArray, vtkVariant>(srcTable, destTable, cid, useVariant))
        {
        return false;
        }
      }
    }

  return true;
}

//----------------------------------------------------------------------------
bool voUtils::transposeTable(vtkTable* table)
{
  vtkNew<vtkTable> transposedTable;
  bool success = voUtils::transposeTable(table, transposedTable.GetPointer());
  if (!success)
    {
    return false;
    }
  table->ShallowCopy(transposedTable.GetPointer());
  return true;
}

//----------------------------------------------------------------------------
bool voUtils::insertColumnIntoTable(vtkTable * table, int position, vtkAbstractArray * columnToInsert)
{
  if (!table)
    {
    return false;
    }
  if (!columnToInsert)
    {
    return false;
    }
  if (table->GetNumberOfRows() != columnToInsert->GetNumberOfComponents() * columnToInsert->GetNumberOfTuples())
    {
    return false;
    }
  if (position < 0)
    {
    position = 0;
    }
  if (position > table->GetNumberOfColumns())
    {
    position = table->GetNumberOfColumns();
    }

  vtkNew<vtkTable> updatedTable;
  for (int cid = 0; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkAbstractArray * column = table->GetColumn(cid);
    Q_ASSERT(column);
    if (cid == position)
      {
      updatedTable->AddColumn(columnToInsert);
      }
    updatedTable->AddColumn(column);
    }
  if (position == table->GetNumberOfColumns())
    {
    updatedTable->AddColumn(columnToInsert);
    }
  table->ShallowCopy(updatedTable.GetPointer());
  return true;
}

//----------------------------------------------------------------------------
void voUtils::setTableColumnNames(vtkTable * table, vtkStringArray * columnNames)
{
  if (!table)
    {
    return;
    }
  if (!columnNames)
    {
    return;
    }

  for (int cid = 0; cid < table->GetNumberOfColumns() && cid < columnNames->GetNumberOfValues(); ++cid)
    {
    vtkAbstractArray * column = table->GetColumn(cid);
    Q_ASSERT(column);
    column->SetName(columnNames->GetValue(cid));
    }
}

//----------------------------------------------------------------------------
bool voUtils::parseRangeString(const QString& rangeString, QList<int>& rangeList, bool alpha)
{
  if(!rangeList.empty())
    {
    return false;
    }

  //Validate - checks for form, not semantics
  QRegExp validRegEx;
  if (!alpha) // Numbers
    {
    validRegEx.setPattern("^\\s*(\\d+\\s*[-,]\\s*)*\\d+\\s*$");
    }
  else // Letters
    {
    validRegEx.setPattern("^\\s*([A-Z]\\s*[-,]\\s*)*[A-Z]\\s*$");
    validRegEx.setCaseSensitivity(Qt::CaseInsensitive);
    }
  if(!validRegEx.exactMatch(rangeString))
    {
    return false;
    }

  //Parse
  QString scratchString(rangeString);
  scratchString.replace(" ", "");
  QStringList rangeStringList = scratchString.split(",");
  rangeStringList.removeDuplicates();

  QRegExp rangeRegEx;
  if (!alpha)
    {
    rangeRegEx.setPattern("^(\\d+)-(\\d+)$");
    }
  else
    {
    rangeRegEx.setPattern("^([A-Z])-([A-Z])$");
    rangeRegEx.setCaseSensitivity(Qt::CaseInsensitive);
    }
  foreach(QString subStr, rangeStringList)
    {
    if(rangeRegEx.indexIn(subStr) != -1)
      {
      int subBegin;
      int subEnd;
      if (!alpha)
        {
        subBegin = rangeRegEx.cap(1).toInt() - 1;
        subEnd = rangeRegEx.cap(2).toInt() - 1;
        }
      else
        {
        subBegin = rangeRegEx.cap(1).toUpper().at(0).toLatin1() - 'A';
        subEnd = rangeRegEx.cap(2).toUpper().at(0).toLatin1() - 'A';
        }
      for(int subCtr = subBegin; subCtr <= subEnd; subCtr++)
        {
        rangeList.push_back(subCtr);
        }
      }
    else
      {
      if (!alpha)
        {
        rangeList.push_back(subStr.toInt() - 1);
        }
      else
        {
        rangeList.push_back(subStr.toUpper().at(0).toLatin1() - 'A');
        }
      }
    }

  return true;
}

