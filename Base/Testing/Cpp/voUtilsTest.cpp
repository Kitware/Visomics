
// Qt includes
#include <QList>
#include <QString>
#include <QStringList>

// Visomics includes
#include "voUtils.h"

// VTK includes
#include <vtkArray.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

// STD includes
#include <cstdlib>

namespace
{

//-----------------------------------------------------------------------------
bool compareArray(vtkArray* array1, vtkArray* array2)
{
  if (array1 == 0 && array2 == 0)
    {
    return true;
    }
  if (array1 == 0 || array2 == 0)
    {
    return false;
    }
  if (array1->GetSize() != array2->GetSize())
    {
    std::cerr << "Compare array Failed !\n"
              << "\tSize(array1): " << array1->GetSize() << "\n"
              << "\tSize(array2): " << array2->GetSize() << std::endl;
    return false;
    }
  for (vtkArray::SizeT i = 0; i < array1->GetSize(); ++i)
    {
    if (array1->GetVariantValueN(i) != array2->GetVariantValueN(i))
      {
      std::cerr << "Compare array Failed !\n"
                << "\tValue(array1): " << array1->GetVariantValueN(i) << "\n"
                << "\tValue(array2): " << array2->GetVariantValueN(i) << std::endl;
      return false;
      }
    }
  return true;
}

//-----------------------------------------------------------------------------
bool compareArray(vtkAbstractArray* array1, vtkAbstractArray* array2, bool compareArrayType = true)
{
  if (array1 == 0 && array2 == 0)
    {
    return true;
    }
  if (array1 == 0 || array2 == 0)
    {
    return false;
    }
  if (compareArrayType && qstrcmp(array1->GetClassName(), array2->GetClassName()) != 0)
    {
    std::cerr << "Compare array Failed !\n"
              << "\tClassName(array1): " << array1->GetClassName() << "\n"
              << "\tClassName(array2): " << array2->GetClassName() << std::endl;
    return false;
    }
  if (array1->GetNumberOfTuples() != array2->GetNumberOfTuples())
    {
    std::cerr << "Compare array Failed !\n"
              << "\tNumberOfTuples(array1): " << array1->GetNumberOfTuples() << "\n"
              << "\tNumberOfTuples(array2): " << array2->GetNumberOfTuples() << std::endl;
    return false;
    }
  if (array1->GetNumberOfComponents() != array2->GetNumberOfComponents())
    {
    std::cerr << "Compare array Failed !\n"
              << "\tNumberOfComponents(array1): " << array1->GetNumberOfComponents() << "\n"
              << "\tNumberOfComponents(array2): " << array2->GetNumberOfComponents() << std::endl;
    return false;
    }
  for (int i = 0; i < array1->GetNumberOfTuples() * array1->GetNumberOfComponents(); ++i)
    {
    vtkVariant v1 = array1->GetVariantValue(i);
    vtkVariant v2 = array2->GetVariantValue(i);
    if (v1.IsDouble() || v1.IsFloat())
      {
      if (vtkMath::IsInf(v1.ToDouble()) && vtkMath::IsInf(v2.ToDouble()))
        {
        continue;
        }
      else if (!qFuzzyCompare(1 + v1.ToDouble(), 1 + v2.ToDouble()))
        {
        std::cerr << "Compare array Failed !\n"
                  << "\tValueAsDouble(table1): " << v1.ToDouble() << "\n"
                  << "\tValueAsDouble(table2): " << v2.ToDouble() << std::endl;
        return false;
        }
      }
    else
      {
      if (v1 != v2)
        {
        std::cerr << "Compare array Failed !\n"
                  << "\tValueAsVariant(table1): " << array1->GetVariantValue(i) << "\n"
                  << "\tValueAsVariant(table2): " << array2->GetVariantValue(i) << std::endl;
        return false;
        }
      }
    }
  return true;
}

//-----------------------------------------------------------------------------
// compareTable() does not and should not compare column names
bool compareTable(int line, vtkTable * outputTable, vtkTable * expectedTable, bool compareArrayType = true)
{
  bool success = true;
  if (outputTable == 0 && expectedTable == 0)
    {
    return true;
    }
  if (outputTable == 0 || expectedTable == 0)
    {
    success = false;
    }

  if (success && (outputTable->GetNumberOfColumns() != expectedTable->GetNumberOfColumns()))
    {
    std::cerr << "Line " << line << " - Problem with transposeTable()" << std::endl;
    std::cerr << "Compare table Failed !\n"
              << "\tNumberOfColumns(outputTable): " << outputTable->GetNumberOfColumns() << "\n"
              << "\tNumberOfColumns(expectedTable): " << expectedTable->GetNumberOfColumns() << std::endl;
    success = false;
    }

  if (success && (outputTable->GetNumberOfRows() != expectedTable->GetNumberOfRows()))
    {
    std::cerr << "Line " << line << " - Problem with transposeTable()" << std::endl;
    std::cerr << "Compare table Failed !\n"
              << "\tNumberOfRows(outputTable): " << outputTable->GetNumberOfRows() << "\n"
              << "\tNumberOfRows(expectedTable): " << expectedTable->GetNumberOfRows() << std::endl;
    success = false;
    }

  if (success)
    {
    for (int cid = 0; cid < outputTable->GetNumberOfColumns(); cid++)
      {
      if (!compareArray(outputTable->GetColumn(cid), expectedTable->GetColumn(cid), compareArrayType))
        {
        std::cerr << "Line " << line << " - Problem with transposeTable()" << std::endl;
        std::cerr << "Compare table Failed !\n"
                  << "\tArray in column "<< cid << " are different" << std::endl;
        success = false;
        break;
        }
      }
    }
  if (!success)
    {
    std::cerr << "outputTable:" << std::endl;
    outputTable->Dump();

    std::cerr << "expectedTable:" << std::endl;
    expectedTable->Dump();
    return false;
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
              << "Problem with counterAlphaToInt()\n"
              << "\tinputString:" << qPrintable(inputString) << "\n"
              << "\tcurrentValue:" << currentValue << "\n"
              << "\texpectedValue:" << expectedValue << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
bool parseRangeStringAlphaTestCase(int line, const QString& inputRangeString, QList<int>& expectedRange)
{
  QList<int> computedRange;
  bool success = voUtils::parseRangeString(inputRangeString, computedRange, /* alpha= */ true);
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with parseRangeString()" << std::endl;
    return false;
    }
  if (expectedRange != computedRange)
    {
    std::cerr << "Line " << line << " - Problem with parseRangeString()\n"
              << "\tinputRangeString:" << qPrintable(inputRangeString) << "\n"
              << "\tCurrent:" << qPrintable(intListToStringList(computedRange).join(",")) << "\n"
              << "\tExpected:" << qPrintable(intListToStringList(expectedRange).join(",")) << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
bool transposeAndCheckResult(int line, const voUtils::TransposeOption& transposeOption,
                             vtkTable * srcTable, vtkTable * transposedTable,
                             int expectedNumberOfRows, int expectedNumberOfColumns
                             /*vtkTable* expectedTransposedTable*/)
{
  bool success = voUtils::transposeTable(srcTable, transposedTable, transposeOption);
  if (!success)
    {
    std::cerr << "Line " << line << " - Problem with transposeTable()" << std::endl;
    }
  if (success && (transposedTable->GetNumberOfRows() != expectedNumberOfRows))
    {
    std::cerr << "Line " << line << " - "
              << "Problem with transposeTable()\n"
              << "\tExpectedNumberOfRows: " << expectedNumberOfRows << "\n"
              << "\tNumberOfRows: " << transposedTable->GetNumberOfRows() << std::endl;
    success = false;
    }
  if (success && (transposedTable->GetNumberOfColumns() != expectedNumberOfColumns))
    {
    std::cerr << "Line " << line << " - "
              << "Problem with transposeTable()\n"
              << "\tExpectedNumberOfColumns: " << expectedNumberOfColumns << "\n"
              << "\tNumberOfColumns: " << transposedTable->GetNumberOfColumns() << std::endl;
    success = false;
    }
  if (!success)
    {
    std::cerr << "srcTable:" << std::endl;
    srcTable->Dump();
    std::cerr << "transposedTable:" << std::endl;
    transposedTable->Dump();
    return false;
    }
  return true;
}

} // end anonymous namespace

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
  stringArray->SetName("stringArray");
  stringArray->InsertValue(0, "zero");
  stringArray->InsertValue(1, "one");
  stringArray->InsertValue(2, "two");
  stringArray->InsertValue(3, "three");
  stringArray->InsertValue(4, "four");
  inputTable->AddColumn(stringArray.GetPointer());

  // Integer column
  vtkNew<vtkIntArray> intArray;
  intArray->SetNumberOfValues(5);
  intArray->SetName("intArray");
  for (int i = 0; i < 5; ++i)
    {
    intArray->InsertValue(i, i);
    }
  inputTable->AddColumn(intArray.GetPointer());

  // Double column
  vtkNew<vtkDoubleArray> doubleArray;
  doubleArray->SetNumberOfValues(5);
  doubleArray->SetName("doubleArray");
  for (double i = 0; i < 5; ++i)
    {
    doubleArray->InsertValue(static_cast<int>(i), i + 0.5);
    }
  inputTable->AddColumn(doubleArray.GetPointer());

  // Variant column
  vtkNew<vtkVariantArray> variantArray;
  variantArray->SetNumberOfValues(5);
  variantArray->SetName("variantArray");
  variantArray->InsertValue(0, vtkVariant(0));
  variantArray->InsertValue(1, vtkVariant('1'));
  variantArray->InsertValue(2, vtkVariant(2.5));
  variantArray->InsertValue(3, vtkVariant('3'));
  variantArray->InsertValue(4, vtkVariant("four"));
  inputTable->AddColumn(variantArray.GetPointer());

  originalTable->DeepCopy(inputTable.GetPointer());

  // inputTable->Dump();

  if (!compareTable(__LINE__, originalTable.GetPointer(), inputTable.GetPointer()))
    {
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test transposeTable(vtkTable * srcTable, vtkTable * destTable, const TransposeOption& transposeOption)
  //  -> transposeOption = WithoutHeaders
  //-----------------------------------------------------------------------------

  bool success = voUtils::transposeTable(0, 0);
  if (success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;
    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> transpose;
  if (!transposeAndCheckResult(__LINE__, voUtils::WithoutHeaders,
                               /* srcTable= */ inputTable.GetPointer(),
                               /* transposedTable= */ transpose.GetPointer(),
                               /* expectedNumberOfRows= */ inputTable->GetNumberOfColumns(),
                               /* expectedNumberOfColumns= */ inputTable->GetNumberOfRows()))
    {
    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> transposeTranspose;
  if (!transposeAndCheckResult(__LINE__, voUtils::WithoutHeaders,
                               /* srcTable= */ transpose.GetPointer(),
                               /* transposedTable= */ transposeTranspose.GetPointer(),
                               /* expectedNumberOfRows= */ transpose->GetNumberOfColumns(),
                               /* expectedNumberOfColumns= */ transpose->GetNumberOfRows()))
    {
    return EXIT_FAILURE;
    }

  // Compare table
  if (!compareTable(__LINE__, originalTable.GetPointer(),
                    transposeTranspose.GetPointer(),
                    /* compareArrayType= */ false))
    {
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test transposeTable(vtkTable * srcTable, vtkTable * destTable, const TransposeOption& transposeOption)
  //  -> transposeOption = FirstColumnIntoColumnNames
  //-----------------------------------------------------------------------------
  vtkNew<vtkTable> transposeFirstColumnIntoColumnNames;
  if (!transposeAndCheckResult(__LINE__, voUtils::FirstColumnIntoColumnNames,
                               /* srcTable= */ inputTable.GetPointer(),
                               /* transposedTable= */ transposeFirstColumnIntoColumnNames.GetPointer(),
                               /* expectedNumberOfRows= */ inputTable->GetNumberOfColumns() - 1,
                               /* expectedNumberOfColumns= */ inputTable->GetNumberOfRows()))
    {
    return EXIT_FAILURE;
    }

  // Expected table
  vtkNew<vtkTable> expectedTransposeTableFirstColumnIntoColumnNames;

  vtkNew<vtkVariantArray> var0Array;
  var0Array->SetNumberOfValues(3);
  var0Array->SetName("zero");
  var0Array->InsertValue(0, vtkVariant(0));
  var0Array->InsertValue(1, vtkVariant(0.5));
  var0Array->InsertValue(2, vtkVariant(0));
  expectedTransposeTableFirstColumnIntoColumnNames->AddColumn(var0Array.GetPointer());

  vtkNew<vtkVariantArray> var1Array;
  var1Array->SetNumberOfValues(3);
  var1Array->SetName("one");
  var1Array->InsertValue(0, vtkVariant(1));
  var1Array->InsertValue(1, vtkVariant(1.5));
  var1Array->InsertValue(2, vtkVariant('1'));
  expectedTransposeTableFirstColumnIntoColumnNames->AddColumn(var1Array.GetPointer());

  vtkNew<vtkVariantArray> var2Array;
  var2Array->SetNumberOfValues(3);
  var2Array->SetName("two");
  var2Array->InsertValue(0, vtkVariant(2));
  var2Array->InsertValue(1, vtkVariant(2.5));
  var2Array->InsertValue(2, vtkVariant(2.5));
  expectedTransposeTableFirstColumnIntoColumnNames->AddColumn(var2Array.GetPointer());

  vtkNew<vtkVariantArray> var3Array;
  var3Array->SetNumberOfValues(3);
  var3Array->SetName("three");
  var3Array->InsertValue(0, vtkVariant(3));
  var3Array->InsertValue(1, vtkVariant(3.5));
  var3Array->InsertValue(2, vtkVariant('3'));
  expectedTransposeTableFirstColumnIntoColumnNames->AddColumn(var3Array.GetPointer());

  vtkNew<vtkVariantArray> var4Array;
  var4Array->SetNumberOfValues(3);
  var4Array->SetName("four");
  var4Array->InsertValue(0, vtkVariant(4));
  var4Array->InsertValue(1, vtkVariant(4.5));
  var4Array->InsertValue(2, vtkVariant("four"));
  expectedTransposeTableFirstColumnIntoColumnNames->AddColumn(var4Array.GetPointer());

  // Compare table
  if (!compareTable(__LINE__, transposeFirstColumnIntoColumnNames.GetPointer(), expectedTransposeTableFirstColumnIntoColumnNames.GetPointer()))
    {
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test transposeTable(vtkTable * srcTable, vtkTable * destTable, const TransposeOption& transposeOption)
  //  -> transposeOption = ColumnNamesIntoFirstColumn
  //-----------------------------------------------------------------------------

  vtkNew<vtkTable> transposeColumnNamesIntoFirstColumn;
  if (!transposeAndCheckResult(__LINE__, voUtils::ColumnNamesIntoFirstColumn,
                               /* srcTable= */ inputTable.GetPointer(),
                               /* transposedTable= */ transposeColumnNamesIntoFirstColumn.GetPointer(),
                               /* expectedNumberOfRows= */ inputTable->GetNumberOfColumns(),
                               /* expectedNumberOfColumns= */ inputTable->GetNumberOfRows() + 1))
    {
    return EXIT_FAILURE;
    }

  // Expected table
  vtkNew<vtkTable> expectedTransposeTableColumnNamesIntoFirstColumn;

  vtkNew<vtkVariantArray> stringArray2;
  stringArray2->SetNumberOfValues(4);
  stringArray2->InsertValue(0, vtkVariant("stringArray"));
  stringArray2->InsertValue(1, vtkVariant("intArray"));
  stringArray2->InsertValue(2, vtkVariant("doubleArray"));
  stringArray2->InsertValue(3, vtkVariant("variantArray"));
  expectedTransposeTableColumnNamesIntoFirstColumn->AddColumn(stringArray2.GetPointer());

  vtkNew<vtkVariantArray> var0Array2;
  var0Array2->SetNumberOfValues(4);
  var0Array2->InsertValue(0, vtkVariant("zero"));
  var0Array2->InsertValue(1, vtkVariant(0));
  var0Array2->InsertValue(2, vtkVariant(0.5));
  var0Array2->InsertValue(3, vtkVariant(0));
  expectedTransposeTableColumnNamesIntoFirstColumn->AddColumn(var0Array2.GetPointer());

  vtkNew<vtkVariantArray> var1Array2;
  var1Array2->SetNumberOfValues(4);
  var1Array2->InsertValue(0, vtkVariant("one"));
  var1Array2->InsertValue(1, vtkVariant(1));
  var1Array2->InsertValue(2, vtkVariant(1.5));
  var1Array2->InsertValue(3, vtkVariant('1'));
  expectedTransposeTableColumnNamesIntoFirstColumn->AddColumn(var1Array2.GetPointer());

  vtkNew<vtkVariantArray> var2Array2;
  var2Array2->SetNumberOfValues(4);
  var2Array2->InsertValue(0, vtkVariant("two"));
  var2Array2->InsertValue(1, vtkVariant(2));
  var2Array2->InsertValue(2, vtkVariant(2.5));
  var2Array2->InsertValue(3, vtkVariant(2.5));
  expectedTransposeTableColumnNamesIntoFirstColumn->AddColumn(var2Array2.GetPointer());

  vtkNew<vtkVariantArray> var3Array2;
  var3Array2->SetNumberOfValues(4);
  var3Array2->InsertValue(0, vtkVariant("three"));
  var3Array2->InsertValue(1, vtkVariant(3));
  var3Array2->InsertValue(2, vtkVariant(3.5));
  var3Array2->InsertValue(3, vtkVariant('3'));
  expectedTransposeTableColumnNamesIntoFirstColumn->AddColumn(var3Array2.GetPointer());

  vtkNew<vtkVariantArray> var4Array2;
  var4Array2->SetNumberOfValues(4);
  var4Array2->InsertValue(0, vtkVariant("four"));
  var4Array2->InsertValue(1, vtkVariant(4));
  var4Array2->InsertValue(2, vtkVariant(4.5));
  var4Array2->InsertValue(3, vtkVariant("four"));
  expectedTransposeTableColumnNamesIntoFirstColumn->AddColumn(var4Array2.GetPointer());

  // Compare table
  if (!compareTable(__LINE__, transposeColumnNamesIntoFirstColumn.GetPointer(),
                    expectedTransposeTableColumnNamesIntoFirstColumn.GetPointer(),
                    /* compareArrayType= */ false))
    {
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test transposeTable(vtkTable * srcTable, vtkTable * destTable, const TransposeOption& transposeOption)
  //  -> transposeOption = Headers
  //-----------------------------------------------------------------------------
  vtkNew<vtkTable> transposeHeaders;
  if (!transposeAndCheckResult(__LINE__, voUtils::Headers,
                               /* srcTable= */ inputTable.GetPointer(),
                               /* transposedTable= */ transposeHeaders.GetPointer(),
                               /* expectedNumberOfRows= */ inputTable->GetNumberOfColumns() - 1,
                               /* expectedNumberOfColumns= */ inputTable->GetNumberOfRows() + 1))
    {
    return EXIT_FAILURE;
    }

  // Expected table
  vtkNew<vtkTable> expectedTransposeTableHeaders;

  vtkNew<vtkVariantArray> stringArray3;
  stringArray3->SetNumberOfValues(3);
  stringArray3->SetName("stringArray");
  stringArray3->InsertValue(0, vtkVariant("intArray"));
  stringArray3->InsertValue(1, vtkVariant("doubleArray"));
  stringArray3->InsertValue(2, vtkVariant("variantArray"));
  expectedTransposeTableHeaders->AddColumn(stringArray3.GetPointer());

  vtkNew<vtkVariantArray> var0Array3;
  var0Array3->SetNumberOfValues(3);
  var0Array3->SetName("zero");
  var0Array3->InsertValue(0, vtkVariant(0));
  var0Array3->InsertValue(1, vtkVariant(0.5));
  var0Array3->InsertValue(2, vtkVariant(0));
  expectedTransposeTableHeaders->AddColumn(var0Array3.GetPointer());

  vtkNew<vtkVariantArray> var1Array3;
  var1Array3->SetNumberOfValues(3);
  var1Array3->SetName("one");
  var1Array3->InsertValue(0, vtkVariant(1));
  var1Array3->InsertValue(1, vtkVariant(1.5));
  var1Array3->InsertValue(2, vtkVariant('1'));
  expectedTransposeTableHeaders->AddColumn(var1Array3.GetPointer());

  vtkNew<vtkVariantArray> var2Array3;
  var2Array3->SetNumberOfValues(3);
  var2Array3->SetName("two");
  var2Array3->InsertValue(0, vtkVariant(2));
  var2Array3->InsertValue(1, vtkVariant(2.5));
  var2Array3->InsertValue(2, vtkVariant(2.5));
  expectedTransposeTableHeaders->AddColumn(var2Array3.GetPointer());

  vtkNew<vtkVariantArray> var3Array3;
  var3Array3->SetNumberOfValues(3);
  var3Array3->SetName("three");
  var3Array3->InsertValue(0, vtkVariant(3));
  var3Array3->InsertValue(1, vtkVariant(3.5));
  var3Array3->InsertValue(2, vtkVariant('3'));
  expectedTransposeTableHeaders->AddColumn(var3Array3.GetPointer());

  vtkNew<vtkVariantArray> var4Array3;
  var4Array3->SetNumberOfValues(3);
  var4Array3->SetName("four");
  var4Array3->InsertValue(0, vtkVariant(4));
  var4Array3->InsertValue(1, vtkVariant(4.5));
  var4Array3->InsertValue(2, vtkVariant("four"));
  expectedTransposeTableHeaders->AddColumn(var4Array3.GetPointer());

  // Compare table
  if (!compareTable(__LINE__, transposeHeaders.GetPointer(),
                    expectedTransposeTableHeaders.GetPointer(),
                    /* compareArrayType= */ false))
    {
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test transposeTable(vtkTable * srcTable, vtkTable * destTable, const TransposeOption& transposeOption)
  //  -> transposeOption = Headers + Check array type
  //-----------------------------------------------------------------------------

  vtkNew<vtkTable> inputDoubleTableWithHeaderColumn;

  vtkNew<vtkStringArray> rowLabelArray;
  rowLabelArray->SetNumberOfValues(5);
  rowLabelArray->SetName("rowLabels");
  for (int rid = 0; rid < 5; ++rid)
    {
    rowLabelArray->InsertValue(rid, QString("row%1").arg(rid).toLatin1());
    }
  inputDoubleTableWithHeaderColumn->AddColumn(rowLabelArray.GetPointer());
  for (int cid = 1; cid < 4; ++cid)
    {
    vtkNew<vtkDoubleArray> doubleArray;
    doubleArray->SetNumberOfValues(5);
    doubleArray->SetName(QString("doubleArray%1").arg(cid).toLatin1());
    for (int rid = 0; rid < 5; ++rid)
      {
      doubleArray->InsertValue(rid, cid + rid + 0.5);
      }
    inputDoubleTableWithHeaderColumn->AddColumn(doubleArray.GetPointer());
    }

  vtkNew<vtkTable> transposedDoubleTableWithHeaderColumn;
  if (!transposeAndCheckResult(__LINE__, voUtils::Headers,
                               /* srcTable= */ inputDoubleTableWithHeaderColumn.GetPointer(),
                               /* transposedTable= */ transposedDoubleTableWithHeaderColumn.GetPointer(),
                               /* expectedNumberOfRows= */ inputTable->GetNumberOfColumns() - 1,
                               /* expectedNumberOfColumns= */ inputTable->GetNumberOfRows() + 1))
    {
    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> expectedTransposeDoubleTableWithHeaderColumn;

  vtkNew<vtkStringArray> rowLabelArray2;
  rowLabelArray2->SetNumberOfValues(3);
  rowLabelArray2->SetName("rowLabels");
  for (int rid = 0; rid < 3; ++rid)
    {
    rowLabelArray2->InsertValue(rid, QString("doubleArray%1").arg(rid + 1).toLatin1());
    }
  expectedTransposeDoubleTableWithHeaderColumn->AddColumn(rowLabelArray2.GetPointer());
  for (int cid = 0; cid < 5; ++cid)
    {
    vtkNew<vtkDoubleArray> doubleArray;
    doubleArray->SetNumberOfValues(3);
    doubleArray->SetName(QString("row%1").arg(cid).toLatin1());
    for (int rid = 0; rid < 3; ++rid)
      {
      doubleArray->SetValue(rid, cid + 1 + rid + 0.5);
      }
    expectedTransposeDoubleTableWithHeaderColumn->AddColumn(doubleArray.GetPointer());
    }

  // Compare table
  if (!compareTable(__LINE__, transposedDoubleTableWithHeaderColumn.GetPointer(),
                    expectedTransposeDoubleTableWithHeaderColumn.GetPointer(),
                    /* compareArrayType= */ true))
    {
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
  if (!compareTable(__LINE__, testTranspose.GetPointer(), transpose.GetPointer()))
    {
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
  if (!compareTable(__LINE__, originalTable.GetPointer(),
                    testTranspose.GetPointer(),
                    /* compareArrayType= */ false))
    {
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test flipTable(vtkTable* table, const FlipOption& flipOption, int offset)
  //-----------------------------------------------------------------------------
  vtkNew<vtkTable> flipTableBaseTable;
  vtkNew<vtkTable> flipTableVerticalFlipExpectedTable;

  flipTableBaseTable->AddColumn(stringArray3.GetPointer());
  flipTableBaseTable->AddColumn(var0Array3.GetPointer());
  flipTableBaseTable->AddColumn(var1Array3.GetPointer());
  flipTableBaseTable->AddColumn(var2Array3.GetPointer());
  flipTableBaseTable->AddColumn(var3Array3.GetPointer());
  flipTableBaseTable->AddColumn(var4Array3.GetPointer());

  flipTableVerticalFlipExpectedTable->AddColumn(stringArray3.GetPointer());
  flipTableVerticalFlipExpectedTable->AddColumn(var4Array3.GetPointer());
  flipTableVerticalFlipExpectedTable->AddColumn(var3Array3.GetPointer());
  flipTableVerticalFlipExpectedTable->AddColumn(var2Array3.GetPointer());
  flipTableVerticalFlipExpectedTable->AddColumn(var1Array3.GetPointer());
  flipTableVerticalFlipExpectedTable->AddColumn(var0Array3.GetPointer());

  success = voUtils::flipTable(flipTableBaseTable.GetPointer(), voUtils::AboutVerticalAxis, 1);
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with flipTable()" << std::endl;
    flipTableBaseTable->Dump();
    return EXIT_FAILURE;
    }

  // Compare table
  if (!compareTable(__LINE__, flipTableBaseTable.GetPointer(), flipTableVerticalFlipExpectedTable.GetPointer()))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with flipTable method !" << std::endl;
    flipTableBaseTable->Dump();
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
  intArray1->SetValue(1, 1);
  intArray1->SetValue(2, 2);
  intArray1->SetValue(3, 3);
  insertTableTest->AddColumn(intArray1.GetPointer());

  vtkNew<vtkIntArray> intArray2;
  intArray2->SetNumberOfValues(4);
  intArray2->SetValue(0, 10);
  intArray2->SetValue(1, 11);
  intArray2->SetValue(2, 12);
  intArray2->SetValue(3, 13);
  insertTableTest->AddColumn(intArray2.GetPointer());

  vtkNew<vtkIntArray> intArray3;
  intArray3->SetNumberOfValues(4);
  intArray3->SetValue(0, 20);
  intArray3->SetValue(1, 21);
  intArray3->SetValue(2, 22);
  intArray3->SetValue(3, 23);
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
  if (!compareTable(__LINE__, insertTableTest.GetPointer(), insertTableTest1.GetPointer()))
    {
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
  if (!success)/*compareArrayType=*/
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
  // Test vtkStringArray * tableColumnNames(vtkTable * table)
  //-----------------------------------------------------------------------------
  voUtils::tableColumnNames(0);

  vtkNew<vtkTable> tableColumnNamesTest;
  tableColumnNamesTest->DeepCopy(originalTable.GetPointer());

  vtkNew<vtkStringArray> expectedColumnNames;
  expectedColumnNames->SetNumberOfValues(4);
  expectedColumnNames->SetValue(0, "String");
  expectedColumnNames->SetValue(2, "Double");
  expectedColumnNames->SetValue(3, "Variant");

  for (int cid = 0; cid < tableColumnNamesTest->GetNumberOfColumns(); ++cid)
    {
    tableColumnNamesTest->GetColumn(cid)->SetName(expectedColumnNames->GetValue(cid));
    }

  vtkSmartPointer<vtkStringArray> currentColumnNames =
      vtkSmartPointer<vtkStringArray>::Take(voUtils::tableColumnNames(tableColumnNamesTest.GetPointer()));

  success = compareArray(currentColumnNames.GetPointer(), expectedColumnNames.GetPointer());
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with tableColumnNames() - "
              << "'currentColumnNames' is different from 'expectedColumnNames'" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test counterIntToAlpha(int intVal), counterAlphaToInt(QString alphaVal)
  //-----------------------------------------------------------------------------

  if (!counterAlphaToIntTestCase(__LINE__, QLatin1String("A"), 0))
    {
    return EXIT_FAILURE;
    }

  if (!counterAlphaToIntTestCase(__LINE__, QLatin1String("Z"), 25))
    {
    return EXIT_FAILURE;
    }

  if (!counterAlphaToIntTestCase(__LINE__, QLatin1String("AA"), 26))
    {
    return EXIT_FAILURE;
    }

  if (!counterAlphaToIntTestCase(__LINE__, QLatin1String("AZ"), 51))
    {
    return EXIT_FAILURE;
    }

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

  QList<int> expectedRange;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String(""), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 0;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("A"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 25;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("Z"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 26;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("AA"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 51;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("AZ"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 0 << 1 << 2 << 3 << 4;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("A,B,C,D,E"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 0 << 1 << 2 << 3 << 4;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("A, B, C ,D ,E "), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 0 << 1 << 2 << 3 << 4;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("A-E"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 0 << 1 << 2 << 3 << 4;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("A  - E   "), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 0 << 2 << 3 << 4 << 26;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("AA,C-E,A"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  expectedRange.clear();
  expectedRange << 0 << 1 << 2 << 3;
  if (!parseRangeStringAlphaTestCase(__LINE__, QLatin1String("A-C,B-D"), expectedRange))
    {
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test tableToArray()
  //-----------------------------------------------------------------------------

  QList<int> intColumns;
  intColumns << 1;
  vtkSmartPointer<vtkArray> convertedIntColumns;
  voUtils::tableToArray(0, convertedIntColumns, intColumns); // Passing a Null source array shouldn't crash
  voUtils::tableToArray(inputTable.GetPointer(), convertedIntColumns, intColumns);

  vtkArray * expectedIntArray = vtkArray::CreateArray(vtkArray::DENSE, VTK_INT);
  expectedIntArray->Resize(vtkArrayRange(0, 5));
  for (vtkArray::SizeT i = 0; i < 5; ++i)
    {
    expectedIntArray->SetVariantValueN(i, i);
    }
  if (!compareArray(convertedIntColumns.GetPointer(), expectedIntArray))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with tableToArray method !" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test arrayToTable()
  //-----------------------------------------------------------------------------

  // Create 2D array
  vtkArray * arrayToTableTestArray = vtkArray::CreateArray(vtkArray::DENSE, VTK_UNSIGNED_INT);
  arrayToTableTestArray->Resize(4, 3);

  // Fill array with data (use intArray objects from prior test)
  QList<vtkIntArray*> intArrayObjectList;
  intArrayObjectList << intArray1.GetPointer() << intArray2.GetPointer() << intArray3.GetPointer();
  int i = 0;
  foreach (vtkIntArray* intArrayObject, intArrayObjectList)
    {
    for (int j = 0; j < 4; j++)
      {
      arrayToTableTestArray->SetVariantValue(j, i, intArrayObject->GetVariantValue(j));
      }
    i++;
    }

  voUtils::arrayToTable(0, 0);  // Passing Null source array or destination table shouldn't crash

  // Convert array to table
  vtkNew<vtkTable> arrayToTableTestTable;
  voUtils::arrayToTable(0, arrayToTableTestTable.GetPointer());  // Passing a Null source array shouldn't crash
  voUtils::arrayToTable(arrayToTableTestArray, arrayToTableTestTable.GetPointer());

  // Compare arrayToTable converted table to table build directly from 1D arrays (created in prior test)
  if (!compareTable(__LINE__, insertTableTest.GetPointer(),
                    arrayToTableTestTable.GetPointer(), /* comparetype= */ false))
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
