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

#ifndef __voNormalization_h
#define __voNormalization_h

// Qt includes
#include <QHash>
#include <QVariant>


class vtkTable;
//template <class Key, class T> class QHash

//------------------------------------------------------------------------------
namespace Normalization
{

  bool applyLog2(vtkTable * dataTable, const QHash<int, QVariant>& settings);

  bool applyQuantile(vtkTable * dataTable, const QHash<int, QVariant>& settings);

} // end of Normalization namespace

#endif
