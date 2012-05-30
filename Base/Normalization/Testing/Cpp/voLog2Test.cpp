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
#include "voNormalization.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkTable.h>

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
int voLog2Test(int /*argc*/, char * /*argv*/ [])
{
  //-----------------------------------------------------------------------------
  // Test log2Normalization(vtkTable* table)
  //-----------------------------------------------------------------------------

  // Input
  vtkNew<vtkTable> log2NormalizationInput;
    {
    vtkNew<vtkDoubleArray> log2NormalizationInputArray;
    log2NormalizationInputArray->SetNumberOfValues(5);
    for (int i = 0; i < 5; ++i)
      {
      log2NormalizationInputArray->InsertValue(i, i);
      }
    log2NormalizationInput->AddColumn(log2NormalizationInputArray.GetPointer());
    }

  // Expected output
  vtkNew<vtkTable> log2NormalizationExpectedOutput;
    {
    vtkNew<vtkDoubleArray> log2NormalizationExpectedOutputArray;
    log2NormalizationExpectedOutputArray->SetNumberOfValues(5);

    log2NormalizationExpectedOutputArray->InsertValue(0, -HUGE_VAL);
    log2NormalizationExpectedOutputArray->InsertValue(1, 0.0);
    log2NormalizationExpectedOutputArray->InsertValue(2, 1.0);
    log2NormalizationExpectedOutputArray->InsertValue(3, 1.584962500721156);
    log2NormalizationExpectedOutputArray->InsertValue(4, 2.0);
    log2NormalizationExpectedOutput->AddColumn(log2NormalizationExpectedOutputArray.GetPointer());
    }

  Normalization::applyLog2(log2NormalizationInput.GetPointer(), QHash<int, QVariant>());

  if (!compareTable(log2NormalizationInput.GetPointer(), log2NormalizationExpectedOutput.GetPointer()))
    {
    // Tables are expected to be equals
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with log2Normalization()" << std::endl;

    std::cerr << "Updated log2NormalizationInput:" << std::endl;
    log2NormalizationInput->Dump();

    std::cerr << "log2NormalizationExpectedOutput:" << std::endl;
    log2NormalizationExpectedOutput->Dump();
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

