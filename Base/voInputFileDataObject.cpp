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
#include <QFileInfo>

// Visomics includes
#include "voInputFileDataObject.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDataObject.h>

class voInputFileDataObjectPrivate
{
public:
  voInputFileDataObjectPrivate();

  QString FileName;
};

// --------------------------------------------------------------------------
// voInputFileDataObjectPrivate methods

// --------------------------------------------------------------------------
voInputFileDataObjectPrivate::voInputFileDataObjectPrivate()
{
}

// --------------------------------------------------------------------------
// voInputFileDataObject methods

// --------------------------------------------------------------------------
voInputFileDataObject::voInputFileDataObject(QObject* newParent) :
    Superclass(newParent), d_ptr(new voInputFileDataObjectPrivate)
{
}

// --------------------------------------------------------------------------
voInputFileDataObject::voInputFileDataObject(const QString& fileName,
                                             vtkDataObject * newData,
                                             QObject* newParent) :
  Superclass(newParent), d_ptr(new voInputFileDataObjectPrivate)
{
  Q_D(voInputFileDataObject);
  this->setName(QFileInfo(fileName).baseName());
  this->setData(newData);
  d->FileName = fileName;
}

// --------------------------------------------------------------------------
voInputFileDataObject::~voInputFileDataObject()
{

}

// --------------------------------------------------------------------------
QString voInputFileDataObject::fileName()const
{
  Q_D(const voInputFileDataObject);
  return d->FileName;
}

// --------------------------------------------------------------------------
void voInputFileDataObject::setFileName(const QString& newFileName)
{
  Q_D(voInputFileDataObject);
  d->FileName = newFileName;
}
