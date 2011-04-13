
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
    // Table are expected to be equals
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
    // Table are expected to be equals
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
    // Table are expected to be equals
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
    // Table are expected to be equals
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with transposeTable()" << std::endl;

    std::cerr << "originalTable:" << std::endl;
    originalTable->Dump();

    std::cerr << "testTranspose:" << std::endl;
    testTranspose->Dump();

    return EXIT_FAILURE;
    }




  return EXIT_SUCCESS;
}

