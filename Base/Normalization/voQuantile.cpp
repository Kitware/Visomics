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

// Qt includes
//#include <QHash>
//#include <QVariant>

// Visomics includes
#include "voNormalization.h"
#include "voUtils.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>

namespace Normalization
{

//------------------------------------------------------------------------------
bool applyQuantile(vtkTable * dataTable, const QHash<int, QVariant>& settings)
{
  Q_UNUSED(settings);
  if (!dataTable)
    {
    return false;
    }

  vtkSmartPointer<vtkArray> inputDataArray;
  bool result = voUtils::tableToArray(dataTable, inputDataArray);
  if (!result)
    {
    return false;
    }

  vtkNew<vtkArrayData> RInputArrayData;
  RInputArrayData->AddArray(inputDataArray);

  // Run R code
  vtkNew<vtkRCalculatorFilter> RCalc;
  RCalc->SetRoutput(0);
  RCalc->SetInputData(RInputArrayData.GetPointer());
  RCalc->PutArray("0", "inputData");
  RCalc->GetArray("outputData","outputData");
  RCalc->SetRscript(QString(
  "library(\"preprocessCore\")\n"
  "outputData <- normalize.quantiles(inputData)"
  ).toLatin1());
  RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(RCalc->GetOutput());
  vtkNew<vtkTable> outputTable;
  voUtils::arrayToTable(outputArrayData->GetArrayByName("outputData"), outputTable.GetPointer());

  dataTable->ShallowCopy(outputTable.GetPointer());
  dataTable->Modified();

  return true;
}

} // end of Normalization namespace
