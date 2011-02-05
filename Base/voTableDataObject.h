
#ifndef __voTableDataObject_h
#define __voTableDataObject_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voDataObject.h"

class voTableDataObjectPrivate;

class voTableDataObject : public voDataObject
{
  Q_OBJECT
public:
  typedef voDataObject Superclass;
  voTableDataObject(QObject* newParent = 0);
  voTableDataObject(const QString& newName, vtkDataObject * newData, QObject* newParent = 0);
  virtual ~voTableDataObject();
  
protected:
  QScopedPointer<voTableDataObjectPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTableDataObject);
  Q_DISABLE_COPY(voTableDataObject);
};

#endif
