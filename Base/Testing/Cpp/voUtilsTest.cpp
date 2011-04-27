
// Qt includes
#include <QList>
#include <QString>
#include <QStringList>

// Visomics includes
#include "voUtils.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

// STD includes
#include <cstdlib>

namespace
{
//-----------------------------------------------------------------------------
bool compareArray(vtkAbstractArray* array1, vtkAbstractArray* array2)
{
  if (array1 == 0 && array2 == 0)
    {
    return true;
    }
  if (array1 == 0 || array2 == 0)
    {
    return false;
    }
  if (array1->GetNumberOfTuples() != array2->GetNumberOfTuples())
    {
    std::cerr << "Compare array Failed !\n"
              << "\tNumberOfTuples(table1): " << array1->GetNumberOfTuples() << "\n"
              << "\tNumberOfTuples(table2): " << array2->GetNumberOfTuples() << std::endl;
    return false;
    }
  if (array1->GetNumberOfComponents() != array2->GetNumberOfComponents())
    {
    std::cerr << "Compare array Failed !\n"
              << "\tNumberOfComponents(table1): " << array1->GetNumberOfComponents() << "\n"
              << "\tNumberOfComponents(table2): " << array2->GetNumberOfComponents() << std::endl;
    return false;
    }
  for (int i = 0; i < array1->GetNumberOfTuples() * array1->GetNumberOfComponents(); ++i)
    {
    if (array1->GetVariantValue(i) != array2->GetVariantValue(i))
      {
      std::cerr << "Compare array Failed !\n"
                << "\tValue(table1): " << array1->GetVariantValue(i) << "\n"
                << "\tValue(table2): " << array2->GetVariantValue(i) << std::endl;
      return false;
      }
    }
  return true;
}

//-----------------------------------------------------------------------------
bool compareTable(vtkTable * table1, vtkTable * table2)
{
  if (table1 == 0 && table2 == 0)
    {
    return true;
    }
  if (table1 == 0 || table2 == 0)
    {
    return false;
    }

  if (table1->GetNumberOfColumns() != table2->GetNumberOfColumns())
    {
    std::cerr << "Compare table Failed !\n"
              << "\tNumberOfColumns(table1): " << table1->GetNumberOfColumns() << "\n"
              << "\tNumberOfColumns(table2): " << table2->GetNumberOfColumns() << std::endl;
    return false;
    }

  if (table1->GetNumberOfRows() != table2->GetNumberOfRows())
    {
    std::cerr << "Compare table Failed !\n"
              << "\tNumberOfRows(table1): " << table1->GetNumberOfRows() << "\n"
              << "\tNumberOfRows(table2): " << table2->GetNumberOfRows() << std::endl;
    return false;
    }

  for (int cid = 0; cid < table1->GetNumberOfColumns(); cid++)
    {
    if (!compareArray(table1->GetColumn(cid), table2->GetColumn(cid)))
      {
      std::cerr << "Compare table Failed !\n"
                << "\tArray in column "<< cid << " are different" << std::endl;
      return false;
      }
    }

return true;
}

//-----------------------------------------------------------------------------
QStringList intListToStringList(const QList<int>& intList)
{
  QStringList strList;
  foreach(int i, intList)
    {
    strList << QString::number(i);
    }
  return strList;
}

//-----------------------------------------------------------------------------
bool counterAlphaToIntTestCase(int line, const QString& inputString, int expectedValue)
{
  int currentValue = voUtils::counterAlphaToInt(inputString);
  if(currentValue != expectedValue)
    {
    std::cerr << "Line " << line << " - "
              << "Problem with counterAlphaToInt() - "
              << "\tinputString:" << qPrintable(inputString) << "\n"
              << "\tcurrentValue:" << currentValue << "\n"
              << "\texpectedValue:" << expectedValue << std::endl;
    return false;
    }
  return true;
}

}

//-----------------------------------------------------------------------------
int voUtilsTest(int /*argc*/, char * /*argv*/ [])
{
  vtkNew<vtkTable> originalTable;

  //-----------------------------------------------------------------------------
  // Input
  //-----------------------------------------------------------------------------
  vtkNew<vtkTable> inputTable;

  // String column
  vtkNew<vtkStringArray> stringArray;
  stringArray->SetNumberOfValues(5);
  stringArray->InsertValue(0, "zero");
  stringArray->InsertValue(1, "one");
  stringArray->InsertValue(2, "two");
  stringArray->InsertValue(3, "three");
  stringArray->InsertValue(4, "four");
  inputTable->AddColumn(stringArray.GetPointer());

  // Integer column
  vtkNew<vtkIntArray> intArray;
  intArray->SetNumberOfValues(5);
  for (int i = 0; i < 5; ++i)
    {
    intArray->InsertValue(i, i);
    }
  inputTable->AddColumn(intArray.GetPointer());

  // Double column
  vtkNew<vtkDoubleArray> doubleArray;
  doubleArray->SetNumberOfValues(5);
  for (double i = 0; i < 5; ++i)
    {
    doubleArray->InsertValue(static_cast<int>(i), i + 0.5);
    }
  inputTable->AddColumn(doubleArray.GetPointer());

  // Variant column
  vtkNew<vtkVariantArray> variantArray;
  variantArray->SetNumberOfValues(5);
  variantArray->InsertValue(0, vtkVariant(0));
  variantArray->InsertValue(1, vtkVariant('1'));
  variantArray->InsertValue(2, vtkVariant(2.5));
  variantArray->InsertValue(3, vtkVariant('3'));
  variantArray->InsertValue(4, vtkVariant("four"));
  inputTable->AddColumn(variantArray.GetPointer());

  originalTable->DeepCopy(inputTable.GetPointer());

  // inputTable->Dump();

  if (!compareTable(originalTable.GetPointer(), inputTable.GetPointer()))
    {
    // Tables are expected to be equals
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with compareTable()" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test transposeTable(vtkTable * srcTable, vtkTable * destTable)
  //-----------------------------------------------------------------------------

  bool success = voUtils::transposeTable(0, 0);
  if (success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;
    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> transpose;
  success = voUtils::transposeTable(inputTable.GetPointer(), transpose.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;

    std::cerr << "inputTable:" << std::endl;
    inputTable->Dump();

    std::cerr << "transpose:" << std::endl;
    transpose->Dump();

    return EXIT_FAILURE;
    }

  if (inputTable->GetNumberOfColumns() != transpose->GetNumberOfRows())
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()\n"
              << "\tNumberOfColumns(inputTable): " << inputTable->GetNumberOfColumns() << "\n"
              << "\tNumberOfRows(transpose): " << transpose->GetNumberOfRows() << std::endl;

    std::cerr << "inputTable:" << std::endl;
    inputTable->Dump();

    std::cerr << "transpose:" << std::endl;
    transpose->Dump();

    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> transposeTranspose;
  success = voUtils::transposeTable(transpose.GetPointer(), transposeTranspose.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;

    std::cerr << "transpose:" << std::endl;
    transpose->Dump();

    std::cerr << "transposeTranspose:" << std::endl;
    transposeTranspose->Dump();

    return EXIT_FAILURE;
    }

  // Compare table
  if (!compareTable(originalTable.GetPointer(), transposeTranspose.GetPointer()))
    {
    // Tables are expected to be equals
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;

    std::cerr << "originalTable:" << std::endl;
    originalTable->Dump();

    std::cerr << "transposeTranspose:" << std::endl;
    transposeTranspose->Dump();

    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test transposeTable(vtkTable * table)
  //-----------------------------------------------------------------------------

  vtkNew<vtkTable> testTranspose;
  testTranspose->DeepCopy(originalTable.GetPointer());

  success = voUtils::transposeTable(testTranspose.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;
    return EXIT_FAILURE;
    }

  // Compare table
  if (!compareTable(testTranspose.GetPointer(), transpose.GetPointer()))
    {
    // Tables are expected to be equals
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;

    std::cerr << "testTranspose:" << std::endl;
    testTranspose->Dump();

    std::cerr << "transpose:" << std::endl;
    transpose->Dump();

    return EXIT_FAILURE;
    }

  success = voUtils::transposeTable(testTranspose.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;
    return EXIT_FAILURE;
    }

  // Compare table
  if (!compareTable(originalTable.GetPointer(), testTranspose.GetPointer()))
    {
    // Tables are expected to be equals
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;

    std::cerr << "originalTable:" << std::endl;
    originalTable->Dump();

    std::cerr << "testTranspose:" << std::endl;
    testTranspose->Dump();

    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test insertColumnIntoTable(vtkTable * table, int position, vtkAbstractArray * column)
  //-----------------------------------------------------------------------------

  vtkNew<vtkStringArray> stringArraytoInsert;
  stringArraytoInsert->SetNumberOfValues(4);
  stringArraytoInsert->SetValue(0, "zero");
  stringArraytoInsert->SetValue(1, "one");
  stringArraytoInsert->SetValue(2, "two");
  stringArraytoInsert->SetValue(3, "three");

  vtkNew<vtkStringArray> stringArraytoInsertInvalid;
  stringArraytoInsertInvalid->SetNumberOfValues(5);
  stringArraytoInsertInvalid->SetValue(0, "zero");
  stringArraytoInsertInvalid->SetValue(1, "one");
  stringArraytoInsertInvalid->SetValue(2, "two");
  stringArraytoInsertInvalid->SetValue(3, "three");
  stringArraytoInsertInvalid->SetValue(4, "four");

  vtkNew<vtkTable> insertTableTest;
  vtkNew<vtkIntArray> intArray1;
  intArray1->SetNumberOfValues(4);
  intArray1->SetValue(0, 0);
  intArray1->SetValue(0, 1);
  intArray1->SetValue(0, 2);
  intArray1->SetValue(0, 3);
  insertTableTest->AddColumn(intArray1.GetPointer());

  vtkNew<vtkIntArray> intArray2;
  intArray2->SetNumberOfValues(4);
  intArray2->SetValue(0, 10);
  intArray2->SetValue(0, 11);
  intArray2->SetValue(0, 12);
  intArray2->SetValue(0, 13);
  insertTableTest->AddColumn(intArray2.GetPointer());

  vtkNew<vtkIntArray> intArray3;
  intArray3->SetNumberOfValues(4);
  intArray3->SetValue(0, 20);
  intArray3->SetValue(0, 21);
  intArray3->SetValue(0, 22);
  intArray3->SetValue(0, 23);
  insertTableTest->AddColumn(intArray3.GetPointer());


  vtkNew<vtkTable> insertTableTest1;
  insertTableTest1->DeepCopy(insertTableTest.GetPointer());

  success = voUtils::insertColumnIntoTable(insertTableTest1.GetPointer(), 0, stringArraytoInsertInvalid.GetPointer());
  if (success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()" << std::endl;
    return EXIT_FAILURE;
    }

  // Compare table
  if (!compareTable(insertTableTest.GetPointer(), insertTableTest1.GetPointer()))
    {
    // Tables are expected to be equals
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;

    std::cerr << "insertTableTest:" << std::endl;
    insertTableTest->Dump();

    std::cerr << "insertTableTest1:" << std::endl;
    insertTableTest1->Dump();

    return EXIT_FAILURE;
    }

  success = voUtils::insertColumnIntoTable(insertTableTest1.GetPointer(), 0, stringArraytoInsert.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()" << std::endl;
    return EXIT_FAILURE;
    }

  if (insertTableTest1->GetNumberOfColumns() != 4)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()\n"
              << "\tExpected NumberOfColumns: " << 4 << "\n"
              << "\tCurrent NumberOfColumns:" << insertTableTest1->GetNumberOfColumns() << std::endl;

    std::cerr << "insertTableTest1:" << std::endl;
    insertTableTest1->Dump();;

    return EXIT_FAILURE;
    }

  success = voUtils::insertColumnIntoTable(insertTableTest1.GetPointer(), -10, stringArraytoInsert.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()" << std::endl;
    return EXIT_FAILURE;
    }

  if (insertTableTest1->GetNumberOfColumns() != 5)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()\n"
              << "\tExpected NumberOfColumns: " << 5 << "\n"
              << "\tCurrent NumberOfColumns:" << insertTableTest1->GetNumberOfColumns() << std::endl;

    std::cerr << "insertTableTest1:" << std::endl;
    insertTableTest1->Dump();;

    return EXIT_FAILURE;
    }

  success = voUtils::insertColumnIntoTable(insertTableTest1.GetPointer(), 3, stringArraytoInsert.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()" << std::endl;
    return EXIT_FAILURE;
    }

  if (insertTableTest1->GetNumberOfColumns() != 6)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()\n"
              << "\tExpected NumberOfColumns: " << 6 << "\n"
              << "\tCurrent NumberOfColumns:" << insertTableTest1->GetNumberOfColumns() << std::endl;

    std::cerr << "insertTableTest1:" << std::endl;
    insertTableTest1->Dump();;

    return EXIT_FAILURE;
    }

  success = voUtils::insertColumnIntoTable(insertTableTest1.GetPointer(), 6, stringArraytoInsert.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()" << std::endl;
    return EXIT_FAILURE;
    }

  if (insertTableTest1->GetNumberOfColumns() != 7)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable()\n"
              << "\tExpected NumberOfColumns: " << 7 << "\n"
              << "\tCurrent NumberOfColumns:" << insertTableTest1->GetNumberOfColumns() << std::endl;

    std::cerr << "insertTableTest1:" << std::endl;
    insertTableTest1->Dump();

    return EXIT_FAILURE;
    }

  QList<vtkStringArray*> stringArrayColumns;
  stringArrayColumns << vtkStringArray::SafeDownCast(insertTableTest1->GetColumn(0));
  stringArrayColumns << vtkStringArray::SafeDownCast(insertTableTest1->GetColumn(1));
  stringArrayColumns << vtkStringArray::SafeDownCast(insertTableTest1->GetColumn(3));
  stringArrayColumns << vtkStringArray::SafeDownCast(insertTableTest1->GetColumn(6));

  for (int i = 0; i < stringArrayColumns.size(); ++i)
    {
    vtkStringArray* column = stringArrayColumns.at(i);
    if (!column)
      {
      std::cerr << "Line " << __LINE__ << " - "
                << "Problem with insertColumnIntoTable() - "
                << "Column: " << i << " is expected to be a vtkStringArray" << std::endl;

      std::cerr << "insertTableTest1:" << std::endl;
      insertTableTest1->Dump();

      return EXIT_FAILURE;
      }
    success = compareArray(column, stringArraytoInsert.GetPointer());
    if (!success)
      {
      std::cerr << "Line " << __LINE__ << " - "
                << "Problem with insertColumnIntoTable() - "
                << "Column: " << i << " is different from 'stringArraytoInsert'" << std::endl;

      std::cerr << "insertTableTest1:" << std::endl;
      insertTableTest1->Dump();

      std::cerr << "stringArraytoInsert:" << std::endl;
      for(int j = 0; j < stringArraytoInsert->GetNumberOfValues(); ++j)
        {
        if (j != 0)
          {
          std::cerr << ", ";
          }
        std::cerr << stringArraytoInsert->GetValue(j);
        }
      std::cerr << std::endl;

      return EXIT_FAILURE;
      }
    }

  //-----------------------------------------------------------------------------
  // Test setTableColumnNames(vtkTable * table, vtkStringArray * columnNames)
  //-----------------------------------------------------------------------------

  voUtils::setTableColumnNames(0, 0);

  vtkNew<vtkTable> setTableColumnNamesTest;
  setTableColumnNamesTest->DeepCopy(originalTable.GetPointer());

  vtkNew<vtkStringArray> columnNames;
  columnNames->SetNumberOfValues(4);
  columnNames->SetValue(0, "String");
  columnNames->SetValue(1, "Integer");
  columnNames->SetValue(2, "Double");
  columnNames->SetValue(3, "Variant");

  voUtils::setTableColumnNames(setTableColumnNamesTest.GetPointer(), columnNames.GetPointer());

  vtkNew<vtkStringArray> setColumnNames;
  setColumnNames->SetNumberOfValues(4);
  for (int cid = 0; cid < setTableColumnNamesTest->GetNumberOfColumns(); ++cid)
    {
    vtkAbstractArray * column = setTableColumnNamesTest->GetColumn(cid);
    setColumnNames->SetValue(cid, column->GetName());
    }

  success = compareArray(columnNames.GetPointer(), setColumnNames.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with insertColumnIntoTable() - "
              << "'columnNames' is different from 'setColumnNames'" << std::endl;

    std::cerr << "columnNames:" << std::endl;
    for(int j = 0; j < columnNames->GetNumberOfValues(); ++j)
      {
      if (j != 0)
        {
        std::cerr << ", ";
        }
      std::cerr << columnNames->GetValue(j);
      }
    std::cerr << std::endl;

    std::cerr << "setColumnNames:" << std::endl;
    for(int j = 0; j < setColumnNames->GetNumberOfValues(); ++j)
      {
      if (j != 0)
        {
        std::cerr << ", ";
        }
      std::cerr << setColumnNames->GetValue(j);
      }
    std::cerr << std::endl;
    }

  //-----------------------------------------------------------------------------
  // Test counterIntToAlpha(int intVal), counterAlphaToInt(QString alphaVal)
  //-----------------------------------------------------------------------------

  if (!counterAlphaToIntTestCase(__LINE__, QLatin1String("ABA"), 728))
    {
    return EXIT_FAILURE;
    }

  success = true;
  for(int i = 680; i < 1000 && success;i++)
    {
    QString alpha = voUtils::counterIntToAlpha(i);
    if (i != voUtils::counterAlphaToInt(alpha))
      {
      std::cerr << "Line " << __LINE__ << " - "
                << "Problem with counterIntToAlpha(), counterAlphaToInt() - "
                << "Functions are not mutually consistent" << std::endl;
      std::cerr << "counterIntToAlpha(" << i << ") : " << voUtils::counterIntToAlpha(i).toLatin1().data() << std::endl;
      std::cerr << "counterAlphaToInt(" << alpha.toLatin1().data() << ") : " << voUtils::counterAlphaToInt(alpha) << std::endl;
      return EXIT_FAILURE;
      }
    }

  // Don't need to also spot test counterIntToAlpha, since we've spot tested
  // counterAlphaToInt and know its consistant with counterIntToAlpha

  //-----------------------------------------------------------------------------
  // Test parseRangeString(const QString& rangeString, QList<int>& rangeList, bool alpha)
  //-----------------------------------------------------------------------------

  QString rangeString; // empty range
  QList<int> expectedRange;

  QList<int> computedRange;
  success = voUtils::parseRangeString(rangeString, computedRange, /* alpha= */ true);
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with parseRangeString()" << std::endl;
    return EXIT_FAILURE;
    }
  if (expectedRange != computedRange)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with parseRangeString()\n"
              << "\tCurrent:" << qPrintable(intListToStringList(computedRange).join(",")) << "\n"
              << "\tExpected:" << qPrintable(intListToStringList(expectedRange).join(",")) << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

