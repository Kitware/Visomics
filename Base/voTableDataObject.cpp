

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

