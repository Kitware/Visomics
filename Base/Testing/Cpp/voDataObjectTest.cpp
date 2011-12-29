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
#include <QCoreApplication>
#include <QList>
#include <QVariant>

// Visomics includes
#include "voDataObject.h"

// VTK includes
#include <vtkDataObject.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{

//-----------------------------------------------------------------------------
class vtkCustomDataObject : public vtkDataObject
{
public:
  static vtkCustomDataObject* New();
  vtkTypeMacro(vtkCustomDataObject, vtkDataObject);

  static int count;

protected:
  vtkCustomDataObject()
    {
    ++count;
    std::cout << "vtkCustomDataObject" << std::endl;
    }

  ~vtkCustomDataObject()
    {
    --count;
    std::cout << "~vtkCustomDataObject" << std::endl;
    }

private:
  vtkCustomDataObject(const vtkCustomDataObject&); // Not implemented
  void operator=(const vtkCustomDataObject&); // Not implemented
};

vtkStandardNewMacro(vtkCustomDataObject);
int vtkCustomDataObject::count = 0;

//-----------------------------------------------------------------------------
template<typename TYPE>
bool checkDataObjectType(const voDataObject& dataObject, const char* typeName)
{
  if (dataObject.type() != typeName)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::type()"
              " - type is " << qPrintable(dataObject.type()) << std::endl;
    return false;
    }

  QVariant temp = dataObject.data();
  if (!temp.canConvert<TYPE>())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
bool checkDataObjectType(const voDataObject& dataObject, QVariant::Type type)
{
  if (dataObject.type() != QVariant::typeToName(type))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::type()"
              " - type is " << qPrintable(dataObject.type()) << std::endl;
    return false;
    }

  if (!dataObject.data().canConvert(type))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return false;
    }
  return true;
}

}

//-----------------------------------------------------------------------------
int voDataObjectTest(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);

  if (vtkCustomDataObject::count != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with vtkCustomDataObject::count"
              << " - count is expted to be null !" << std::endl;
    return EXIT_FAILURE;
    }

  { // start local scope

  voDataObject dataObject;

  if (!dataObject.type().isEmpty())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::type()"
                 " - type is " << qPrintable(dataObject.type()) << std::endl;
    return EXIT_FAILURE;
    }

  if (dataObject.isVTKDataObject())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::isVTKDataObject()" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Set / Get QVariantList
  //-----------------------------------------------------------------------------
  QVariantList list;
  list << 1 << 2;
  dataObject.setData(QVariant(list));

  if (!checkDataObjectType(dataObject, QVariant::List))
    {
    return EXIT_FAILURE;
    }

  QVariantList storedList = dataObject.data().toList();
  if (storedList.size() != 2)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return EXIT_FAILURE;
    }

  if (dataObject.property("data").toList() != list)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Set / Get QString
  //-----------------------------------------------------------------------------
  QString string("zen");
  dataObject.setData(string);

  if (!checkDataObjectType(dataObject, QVariant::String))
    {
    return EXIT_FAILURE;
    }

  QString storedString = dataObject.data().toString();
  if (storedString != QLatin1String("zen"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return EXIT_FAILURE;
    }

  if (dataObject.property("data").toString() != string)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Set / Get QObject*
  //-----------------------------------------------------------------------------
  QObject * object = new QObject;
  object->setProperty("foo", QLatin1String("bar"));

  dataObject.setData(QVariant::fromValue(object));

  if (!checkDataObjectType<QObject*>(dataObject, QMetaType::typeName(QMetaType::QObjectStar)))
    {
    return EXIT_FAILURE;
    }

  QObject* storedObject = dataObject.data().value<QObject*>();
  if (!storedObject)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return EXIT_FAILURE;
    }
  if (storedObject->property("foo") != QLatin1String("bar"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::data()" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Set / Get vtkDataObject*
  //-----------------------------------------------------------------------------
  vtkNew<vtkCustomDataObject> customDataObject;

  dataObject.setData(customDataObject.GetPointer());

  if (!dataObject.isVTKDataObject())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::isVTKDataObject()" << std::endl;
    return EXIT_FAILURE;
    }

  if (!checkDataObjectType<vtkVariant>(dataObject, "vtkCustomDataObject"))
    {
    return EXIT_FAILURE;
    }

  vtkCustomDataObject * storedCustomDataObject = vtkCustomDataObject::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (storedCustomDataObject != customDataObject.GetPointer())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voDataObject::dataAsVTKDataObject()" << std::endl;
    return EXIT_FAILURE;
    }

  } // end local scope

  if (vtkCustomDataObject::count != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with vtkCustomDataObject::count"
              << " - count is expted to be null !" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
