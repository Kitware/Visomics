
// Qt includes
#include <QUuid>

// Visomics includes
#include "voDataObject.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDataObject.h>

class voDataObjectPrivate
{
public:
  voDataObjectPrivate();

  vtkSmartPointer<vtkDataObject> Data;
  QString                        Name;
  QString                        Uuid;
};

// --------------------------------------------------------------------------
// voDataObjectPrivate methods

// --------------------------------------------------------------------------
voDataObjectPrivate::voDataObjectPrivate()
{
  this->Uuid = QUuid::createUuid().toString();
}
  
// --------------------------------------------------------------------------
// voDataObject methods

// --------------------------------------------------------------------------
voDataObject::voDataObject(QObject* newParent) :
    Superclass(newParent), d_ptr(new voDataObjectPrivate)
{
}

// --------------------------------------------------------------------------
voDataObject::voDataObject(const QString& newName, vtkDataObject * newData, QObject* newParent):
    Superclass(newParent), d_ptr(new voDataObjectPrivate)
{
  Q_D(voDataObject);
  d->Name = newName;
  d->Data = newData;
}

// --------------------------------------------------------------------------
voDataObject::~voDataObject()
{
}

// --------------------------------------------------------------------------
QString voDataObject::name()const
{
  Q_D(const voDataObject);
  return d->Name;
}

// --------------------------------------------------------------------------
void voDataObject::setName(const QString& newName)
{
  Q_D(voDataObject);
  d->Name = newName;
}

// --------------------------------------------------------------------------
QString voDataObject::type()const
{
  Q_D(const voDataObject);
  if (!d->Data)
    {
    return QString();
    }
  return QLatin1String(d->Data->GetClassName());
}

// --------------------------------------------------------------------------
QString voDataObject::uuid()const
{
  Q_D(const voDataObject);
  return d->Uuid;
}

// --------------------------------------------------------------------------  
vtkDataObject* voDataObject::data()const
{
  Q_D(const voDataObject);
  return d->Data;
}
  
// --------------------------------------------------------------------------
void voDataObject::setData(vtkDataObject * newData)
{
  Q_D(voDataObject);
  d->Data = newData;
}
