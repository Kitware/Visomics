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

#ifndef __voQObjectFactory_h
#define __voQObjectFactory_h

// Qt includes
#include <QString>
#include <QStringList>
#include <QHash>

//----------------------------------------------------------------------------
namespace{
template<typename BaseClassType, typename ClassType>
BaseClassType *CreateObject(const QString& newObjectName)
{
  ClassType * object = new ClassType;
  object->setObjectName(newObjectName);
  return object;
}
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
class voQObjectFactory
{
protected:
  typedef BaseClassType *(*CreateObjectFunc)(const QString&);

public:
  typedef typename QHash<QString, CreateObjectFunc>::const_iterator ConstIterator;
  typedef typename QHash<QString, CreateObjectFunc>::iterator Iterator;

  voQObjectFactory(){}
  virtual ~voQObjectFactory(){}

  /// Return list of registered object keys
  QStringList registeredObjectKeys() const
    {
    return this->RegisteredObjectMap.keys();
    }

  /// Register an object in the factory
  template<typename ClassType>
  void registerQObject()
    {
    this->registerObject<ClassType>(ClassType::staticMetaObject.className());
    }

  /// Register an object in the factory
  template<typename ClassType>
  void registerObject(const QString& uniqueId)
    {
    this->RegisteredObjectMap[ uniqueId ] = &CreateObject<BaseClassType, ClassType>;
    }

  /// Create an instance of the object
  BaseClassType *Create(const QString& uniqueId)
    {
    ConstIterator iter = this->RegisteredObjectMap.find(uniqueId);

    if ( iter == this->RegisteredObjectMap.constEnd())
      {
      return 0;
      }
    return (iter.value())(uniqueId);
    }

private:
  QHash<QString, CreateObjectFunc> RegisteredObjectMap;
};

#endif
