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

#ifndef __voTableDataObject_h
#define __voTableDataObject_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voDataObject.h"

class voTableDataObjectPrivate;

class voTableDataObject : public voDataObject
{
  Q_OBJECT
public:
  typedef voDataObject Superclass;
  voTableDataObject(QObject* newParent = 0);
  voTableDataObject(const QString& newName, vtkDataObject * newData, bool newSortable = false, QObject* newParent = 0);
  virtual ~voTableDataObject();

  bool sortable()const;
  void setSortable(const bool& newSortable);

protected:
  QScopedPointer<voTableDataObjectPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTableDataObject);
  Q_DISABLE_COPY(voTableDataObject);
};

#endif
