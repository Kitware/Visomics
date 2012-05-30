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

#ifndef __voRegistry_h
#define __voRegistry_h

// Qt includes
#include <QHash>
#include <QScopedPointer>
#include <QtGlobal>
#include <QVariant>

class QString;
class voRegistryPrivate;
class vtkTable;

class voRegistry
{
public:
  voRegistry();
  virtual ~voRegistry();

  typedef bool(*ApplyNormalizationFunction)(vtkTable*, const QHash<int, QVariant>&);
  void registerMethod(const QString& methodName, ApplyNormalizationFunction function);

  bool apply(const QString& methodName, vtkTable * dataTable, const QHash<int, QVariant>& settings);

protected:
  QScopedPointer<voRegistryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voRegistry);
  Q_DISABLE_COPY(voRegistry);
};


#endif

