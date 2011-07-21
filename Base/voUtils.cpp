
// Qt includes
#include <QDebug>
#include <QtGlobal>
#include <QStringList>
#include <QRegExp>
#include <QSet>

// Visomics includes
#include "voUtils.h"

// VTK includes
#include <vtkArray.h>
#include <vtkArrayToTable.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToArray.h>
#include <vtkVariantArray.h>

namespace
{
//----------------------------------------------------------------------------
template<typename ArrayType, typename ValueType>
bool transposeColumn(vtkTable* srcTable, vtkTable* destTable, int columnId,
                     bool useVariant = false,
                     const voUtils::TransposeOption& transposeOption = voUtils::WithoutHeaders)
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

  int numberOfRowsInTransposedColumn = srcTable->GetNumberOfColumns();
  if (transposeOption & voUtils::FirstColumnIntoColumnNames)
    {
    if (columnId == 0)
      {
      return true;
      }
    columnId--;
    numberOfRowsInTransposedColumn--;
    }

  for (int rid = 0; rid < column->GetNumberOfTuples() * column->GetNumberOfComponents(); ++rid)
    {
    vtkSmartPointer<ArrayType> transposedColumn;
    if (columnId == 0)
      {
      transposedColumn = vtkSmartPointer<ArrayType>::New();
      transposedColumn->SetNumberOfValues(numberOfRowsInTransposedColumn);
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
bool voUtils::transposeTable(vtkTable* srcTable, vtkTable* destTable, const TransposeOption& transposeOption)
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

  int cidOffset = 0;
  if (transposeOption & voUtils::FirstColumnIntoColumnNames)
    {
    cidOffset = 1;
    }

  // Check if column are all from the same type
  bool useVariant = false;
  vtkAbstractArray * firstColumn = srcTable->GetColumn(cidOffset);
  for (int cid = cidOffset; cid < srcTable->GetNumberOfColumns(); ++cid)
    {
    if (qstrcmp(firstColumn->GetClassName(), srcTable->GetColumn(cid)->GetClassName()) != 0)
      {
      useVariant = true;
      break;
      }
    }
  for(int cid = cidOffset; cid < srcTable->GetNumberOfColumns(); ++cid)
    {
    if (!useVariant)
      {
      if (!transposeColumn<vtkDoubleArray, double>(srcTable, destTable, cid, useVariant, transposeOption))
        {
        if (!transposeColumn<vtkIntArray, int>(srcTable, destTable, cid, useVariant, transposeOption))
          {
          if (!transposeColumn<vtkStringArray, vtkStdString>(srcTable, destTable, cid, useVariant, transposeOption))
            {
            if (!transposeColumn<vtkVariantArray, vtkVariant>(srcTable, destTable, cid, useVariant, transposeOption))
              {
              return false;
              }
            }
          }
        }
      }
    else
      {
      if (!transposeColumn<vtkVariantArray, vtkVariant>(srcTable, destTable, cid, useVariant, transposeOption))
        {
        return false;
        }
      }
    }

  // Set columnName on transposed table
  if (transposeOption & voUtils::FirstColumnIntoColumnNames)
    {
    vtkAbstractArray * firstColumn = srcTable->GetColumn(0);
    for (int rid = 0; rid < firstColumn->GetNumberOfComponents() * firstColumn->GetNumberOfTuples(); ++rid)
      {
      vtkAbstractArray * destColumn = destTable->GetColumn(rid);
      destColumn->SetName(firstColumn->GetVariantValue(rid).ToString());
      }
    }

  // Create and insert the columnName
  // TODO Possible optimization could be to embed the following operation
  //      within the first "for" loop.
  if (transposeOption & voUtils::ColumnNamesIntoFirstColumn)
    {
    vtkNew<vtkStringArray> stringArray;
    if (transposeOption & voUtils::FirstColumnIntoColumnNames)
      {
      stringArray->SetName(srcTable->GetColumn(0)->GetName());
      }
    stringArray->SetNumberOfValues(srcTable->GetNumberOfColumns() - cidOffset);
    for(int cid = cidOffset; cid < srcTable->GetNumberOfColumns(); ++cid)
      {
      stringArray->SetValue(cid - cidOffset, srcTable->GetColumn(cid)->GetName());
      }
    voUtils::insertColumnIntoTable(destTable, 0, stringArray.GetPointer());
    }

  return true;
}

//----------------------------------------------------------------------------
bool voUtils::transposeTable(vtkTable* table, const TransposeOption& transposeOption)
{
  vtkNew<vtkTable> transposedTable;
  bool success = voUtils::transposeTable(table, transposedTable.GetPointer(), transposeOption);
  if (!success)
    {
    return false;
    }
  table->ShallowCopy(transposedTable.GetPointer());
  return true;
}

//----------------------------------------------------------------------------
bool voUtils::flipTable(vtkTable* srcTable, vtkTable* destTable, const FlipOption& flipOption, int horizontalOffset, int verticalOffset)
{
  if (!srcTable)
    {
    return false;
    }
  if (!destTable)
    {
    return false;
    }
  if(horizontalOffset < 0 || verticalOffset < 0)
    {
    return false;
    }
  if((flipOption & voUtils::FlipVerticalAxis) && verticalOffset >=  srcTable->GetNumberOfRows())
    {
    return false;
    }
  if((flipOption & voUtils::FlipHorizontalAxis) && horizontalOffset >=  srcTable->GetNumberOfColumns())
    {
    return false;
    }

  // This will only support flipping columns with the same data type, which should be all
  // that's required for typical use
  // To support heterogeneous column types, we must rebuild a new table from scratch, instead of
  // just deep copying the old one
  destTable->DeepCopy(srcTable);

  vtkVariant tempVariant;
  vtkStdString tempString;

  if(flipOption & voUtils::FlipVerticalAxis) // Top - bottom
    {
    for(int topRid = verticalOffset; topRid <= (destTable->GetNumberOfRows() - verticalOffset) / 2; topRid++)
      {
      int bottomRid = destTable->GetNumberOfRows() - (topRid - verticalOffset) - 1;
      for(int cid = 0; cid < destTable->GetNumberOfColumns(); cid++)
        {
        tempVariant = destTable->GetValue(topRid, cid);
        destTable->SetValue(topRid, cid, destTable->GetValue(bottomRid, cid));
        destTable->SetValue(bottomRid, cid, tempVariant);
        }
      }
    }

  if(flipOption & voUtils::FlipHorizontalAxis) // Left - right
    {
    for(int leftCid = horizontalOffset; leftCid <= (srcTable->GetNumberOfColumns() - horizontalOffset) / 2; leftCid++)
      {
      int rightCid = srcTable->GetNumberOfColumns() - (leftCid - horizontalOffset) - 1;
      if(srcTable->GetColumn(leftCid)->GetDataType() != srcTable->GetColumn(leftCid)->GetDataType())
        {
        qWarning() << "Warning in voUtils::flipTable() - attemping to swap columns of different data types";
        return false;
        }
      tempString = srcTable->GetColumn(leftCid)->GetName();
      destTable->GetColumn(leftCid)->SetName(srcTable->GetColumn(rightCid)->GetName());
      destTable->GetColumn(rightCid)->SetName(tempString.c_str());
      for(int rid = 0; rid < srcTable->GetNumberOfRows(); rid++)
        {
        tempVariant = srcTable->GetValue(rid, leftCid);
        destTable->SetValue(rid, leftCid, srcTable->GetValue(rid, rightCid));
        destTable->SetValue(rid, rightCid, tempVariant);
        }
      }
    }

  return true;
}

//----------------------------------------------------------------------------
bool voUtils::flipTable(vtkTable* table, const FlipOption& flipOption, int horizontalOffset, int verticalOffset)
{
  vtkNew<vtkTable> flippedTable;
  bool success = voUtils::flipTable(table, flippedTable.GetPointer(), flipOption, horizontalOffset, verticalOffset);
  if (!success)
    {
    return false;
    }
  table->ShallowCopy(flippedTable.GetPointer());
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
  if ((table->GetNumberOfRows() != columnToInsert->GetNumberOfComponents() * columnToInsert->GetNumberOfTuples()) &&
      (table->GetNumberOfRows() != 0))
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
vtkStringArray* voUtils::tableColumnNames(vtkTable * table, int offset)
{
  if (!table)
    {
    return 0;
    }
  if (offset > table->GetNumberOfColumns())
    {
    return 0;
    }
  vtkStringArray * columnNames = vtkStringArray::New();
  columnNames->SetNumberOfValues(table->GetNumberOfColumns() - offset);

  for (int cid = offset; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkAbstractArray * column = table->GetColumn(cid);
    columnNames->SetValue((cid - offset), column->GetName());
    }
  return columnNames;
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

  if(rangeString.isEmpty())
    {
    return true;
    }

  // Validate - checks for form, not semantics
  QRegExp validRegEx;
  if (!alpha) // Numbers
    {
    validRegEx.setPattern("^(\\d+[-,])*\\d+$");
    }
  else // Letters
    {
    validRegEx.setPattern("^([A-Z]+[-,])*[A-Z]+$");
    validRegEx.setCaseSensitivity(Qt::CaseInsensitive);
    }
  QString scratchString(rangeString);
  scratchString.replace(" ", "");
  if(!validRegEx.exactMatch(scratchString))
    {
    return false;
    }

  // Parse
  QStringList rangeStringList = scratchString.split(",");
  rangeStringList.removeDuplicates();

  QRegExp rangeRegEx;
  if (!alpha)
    {
    rangeRegEx.setPattern("^(\\d+)-(\\d+)$");
    }
  else
    {
    rangeRegEx.setPattern("^([A-Z]+)-([A-Z]+)$");
    rangeRegEx.setCaseSensitivity(Qt::CaseInsensitive);
    }
  foreach(const QString& subStr, rangeStringList)
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
        subBegin = voUtils::counterAlphaToInt(rangeRegEx.cap(1));
        subEnd = voUtils::counterAlphaToInt(rangeRegEx.cap(2));
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
        rangeList.push_back(voUtils::counterAlphaToInt(subStr));
        }
      }
    }

  rangeList = rangeList.toSet().toList(); // Remove duplicates
  qSort(rangeList);

  return true;
}

//----------------------------------------------------------------------------
QString voUtils::counterIntToAlpha(int intVal)
{
  if (intVal < 0)
    {
    return QString();
    }
  else if (intVal < 26)
    {
    return QString(QChar('A' + intVal));
    }
  else
    {
    return voUtils::counterIntToAlpha((intVal / 26) - 1) + voUtils::counterIntToAlpha(intVal % 26);
    }
}

//----------------------------------------------------------------------------
int voUtils::counterAlphaToInt(const QString& alphaVal)
{
  if (alphaVal.length() < 1)
    {
    return -1;
    }
  else if (alphaVal.length() == 1)
    {
    return static_cast<int>(alphaVal.toUpper().at(0).toLatin1() - 'A');
    }
  else
    {
    return (voUtils::counterAlphaToInt(alphaVal.mid(0, alphaVal.length()-1)) + 1) * 26
           + voUtils::counterAlphaToInt(alphaVal.mid(alphaVal.length()-1));
    }
}

//----------------------------------------------------------------------------
bool voUtils::tableToArray(vtkTable* srcTable, vtkSmartPointer<vtkArray>& destArray, const QList<int>& columnList)
{
  if (!srcTable)
    {
    return false;
    }

  vtkNew<vtkTableToArray> tabToArr;
  tabToArr->SetInputConnection(srcTable->GetProducerPort());

  foreach (int ctr, columnList)
    {
    if(ctr < 0 || ctr >= srcTable->GetNumberOfColumns())
      {
      return false;
      }
    tabToArr->AddColumn(ctr);
    }
  tabToArr->Update();

  // Reference count will be incremented
  destArray = tabToArr->GetOutput()->GetArray(0);
  return true;
}

//----------------------------------------------------------------------------
void voUtils::arrayToTable(vtkArray* srcArray, vtkTable* destTable)
{
  if (!srcArray || !destTable)
    {
    return;
    }

  vtkNew<vtkArrayData> arrData;
  arrData->AddArray(srcArray);

  vtkNew<vtkArrayToTable> arrToTab;
  arrToTab->SetInputConnection(arrData->GetProducerPort());
  arrToTab->Update();

  destTable->ShallowCopy(arrToTab->GetOutput());
}

