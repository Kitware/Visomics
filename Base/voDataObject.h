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

#ifndef __voDataObject_h
#define __voDataObject_h

// Qt includes
#include <QMetaType>
#include <QScopedPointer>
#include <QSharedData>
#include <QObject>
#include <QString>

// VTK includes
#include <vtkVariant.h>

class QVariant;
class voDataObjectPrivate;
class vtkDataObject;

class voDataObject : public QObject, public QSharedData
{
  Q_OBJECT
  Q_PROPERTY(QString type READ type)
  Q_PROPERTY(QString uuid READ uuid)
public:
  typedef QObject Superclass;
  voDataObject(QObject* newParent = 0);
  voDataObject(const QString& newName, const QVariant& newData, QObject* newParent = 0);
  voDataObject(const QString& newName, vtkDataObject * newData, QObject* newParent = 0);
  virtual ~voDataObject();
  
  QString name()const;
  void setName(const QString& newName);

  QString type()const;

  QString uuid()const;
  
  QVariant data()const;
  void setData(const QVariant& newData);

  static vtkDataObject * toVTKDataObject(voDataObject* dataObject);

  vtkDataObject* dataAsVTKDataObject()const;

  void setData(vtkDataObject * newData);

  static bool isVTKDataObject(voDataObject * dataObject);

  bool isVTKDataObject()const;

protected:
  QScopedPointer<voDataObjectPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDataObject);
  Q_DISABLE_COPY(voDataObject);
};

Q_DECLARE_METATYPE(vtkVariant);

#endif
