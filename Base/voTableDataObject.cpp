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
#include "voTableDataObject.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDataObject.h>

class voTableDataObjectPrivate
{
public:
  voTableDataObjectPrivate();
};

// --------------------------------------------------------------------------
// voTableDataObjectPrivate methods

// --------------------------------------------------------------------------
voTableDataObjectPrivate::voTableDataObjectPrivate()
{
}
  
// --------------------------------------------------------------------------
// voTableDataObject methods

// --------------------------------------------------------------------------
voTableDataObject::voTableDataObject(QObject* newParent) :
    Superclass(newParent), d_ptr(new voTableDataObjectPrivate)
{
}

// --------------------------------------------------------------------------
voTableDataObject::voTableDataObject(const QString& newName, vtkDataObject * newData, QObject* newParent):
    Superclass(newName, newData, newParent), d_ptr(new voTableDataObjectPrivate)
{
}

// --------------------------------------------------------------------------
voTableDataObject::~voTableDataObject()
{

}

