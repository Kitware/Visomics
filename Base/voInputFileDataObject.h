
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
