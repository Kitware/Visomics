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

#ifndef __voInputFileDataObject_h
#define __voInputFileDataObject_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voDataObject.h"

class voInputFileDataObjectPrivate;

class voInputFileDataObject : public voDataObject
{
  Q_OBJECT
public:
  typedef voDataObject Superclass;
  voInputFileDataObject(QObject* newParent = 0);
  voInputFileDataObject(const QString& fileName, vtkDataObject * newData, QObject* newParent = 0);
  virtual ~voInputFileDataObject();

  QString fileName()const;
  void setFileName(const QString& newFileName);
  
protected:
  QScopedPointer<voInputFileDataObjectPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voInputFileDataObject);
  Q_DISABLE_COPY(voInputFileDataObject);
};

#endif
