
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

