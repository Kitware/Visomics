
#ifndef __voDataObject_h
#define __voDataObject_h

// Qt includes
#include <QScopedPointer>
#include <QSharedData>
#include <QObject>
#include <QString>

class voDataObjectPrivate;
class vtkDataObject;

class voDataObject : public QObject, public QSharedData
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  voDataObject(QObject* newParent = 0);
  voDataObject(const QString& newName, vtkDataObject * newData, QObject* newParent = 0);
  virtual ~voDataObject();
  
  QString name()const;
  void setName(const QString& newName);

  QString type()const;

  QString uuid()const;
  
  vtkDataObject* data()const;
  void setData(vtkDataObject * newData);

protected:
  QScopedPointer<voDataObjectPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDataObject);
  Q_DISABLE_COPY(voDataObject);
};

#endif
