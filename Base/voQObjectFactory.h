
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
