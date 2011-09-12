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
#include "voRegistry.h"

// VTK includes
#include <vtkTable.h>

//----------------------------------------------------------------------------
namespace
{
bool applyNoop(vtkTable * /*dataTable*/, const QHash<int, QVariant>& /*settings*/){ return true; }
} // end of Normalization namespace

//----------------------------------------------------------------------------
class voRegistryPrivate
{
public:
  QHash<QString, voRegistry::ApplyNormalizationFunction> MethodNameToFunctionMap;
};

//----------------------------------------------------------------------------
// voRegistry methods

//----------------------------------------------------------------------------
voRegistry::voRegistry():d_ptr(new voRegistryPrivate)
{
  this->registerMethod("No", applyNoop);
}

//----------------------------------------------------------------------------
voRegistry::~voRegistry()
{
}

//----------------------------------------------------------------------------
void voRegistry::registerMethod(const QString& methodName, ApplyNormalizationFunction function)
{
  Q_D(voRegistry);
  if (d->MethodNameToFunctionMap.keys().contains(methodName))
    {
    return;
    }
  if (!function)
    {
    return;
    }
  d->MethodNameToFunctionMap.insert(methodName, function);
}

//----------------------------------------------------------------------------  
bool voRegistry::apply(const QString& methodName, vtkTable * dataTable, const QHash<int, QVariant>& settings)
{
  Q_D(voRegistry);
  if (!d->MethodNameToFunctionMap.keys().contains(methodName))
    {
    return false;
    }
  return (*d->MethodNameToFunctionMap.value(methodName))(dataTable, settings);
}
