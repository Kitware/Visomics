
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
