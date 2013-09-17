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
#include <QVariant>
#include <QtVariantProperty>
#include <QtVariantPropertyManager>
#include <QSet>

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
  this->setData(newData);

  bool fileExists = QFileInfo(fileName).exists();
  if (fileExists)
    {
    d->FileName = fileName;
    }

  QtVariantPropertyManager* variantPropertyManager =
    this->variantPropertyManager();

  // set name and property "Name"
  QString dataType = newData->GetClassName();
  QtVariantProperty * nameProperty =
    variantPropertyManager->addProperty(QVariant::String,"Name");
  if (!fileExists)
    {
    this->setName(fileName);
    nameProperty->setValue(fileName);
    }
  else if (dataType == "vtkTree")
    {
    this->setName("Tree");
    nameProperty->setValue("Tree");
    }
  else if (dataType == "vtkExtendedTable")
    {
    this->setName("Table");
    nameProperty->setValue("Table");
    }
  else
    {
    this->setName("Input Data");
    nameProperty->setValue("Input Data");
    }

  // set property "Data Type"
  QtVariantProperty * dataTypeProperty =
    variantPropertyManager->addProperty(QVariant::String, "Data Type");
  dataTypeProperty->setValue(dataType);

  if (fileExists)
    {
    // set property "File Name"
    QtVariantProperty * fileNameProperty =
      variantPropertyManager->addProperty(QVariant::String,"File Name");
    fileNameProperty->setValue(fileName);
    }
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

  //set property "Name"
  QtVariantPropertyManager* VariantPropertyManager = this->variantPropertyManager();
  QSet<QtProperty*> displayProperties = VariantPropertyManager->properties();
  foreach(QtProperty * prop, displayProperties)
    {
    QtVariantProperty * variantProp = dynamic_cast<QtVariantProperty*> (prop);
    if (variantProp->propertyName() == "File Name")
      {
       variantProp->setValue(newFileName);
      }
    }
}
