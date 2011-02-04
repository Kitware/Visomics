
#ifndef __voDataModelItem_h
#define __voDataModelItem_h

// Qt includes
#include <QObject>
#include <QScopedPointer>
#include <QStandardItem>
#include <QVariant>

// Visomics includes
#include "voDataModel.h"

class voDataModelItemPrivate;
class voDataObject;

class voDataModelItem : public QStandardItem
{
public:

  typedef QStandardItem Superclass;
  voDataModelItem(voDataObject * newDataObject, int newColumn = voDataModel::NameColumn);
  voDataModelItem(const QString& newText, int newColumn = voDataModel::NameColumn);
  virtual ~voDataModelItem();

  enum DataModelItemRole
    {
    ViewTypeRole = Qt::UserRole + 1,
    RawViewTypeRole,
    UuidRole,
    IsAnalysisContainerRole,
    AnalysisVoidStarRole,
    OutputNameRole,
    };

  QString viewType()const;
  void setViewType(const QString& newViewType);

  QString rawViewType()const;
  void setRawViewType(const QString& newRawViewType);

  enum DataModelItemType
    {
    Unspecified = QStandardItem::UserType + 1,
    ContainerType,
    InputType,
    OutputType,
    ViewType
    };

  QString uuid() const;

  virtual int type() const;
  void setType(int newType);

  static int typeFromDataObject(voDataObject * newDataObject);

  QVariant data(int role = Qt::UserRole + 1) const;

  void setDataObject(voDataObject* newDataObject);
  voDataObject * dataObject() const;
  
protected:
  QScopedPointer<voDataModelItemPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDataModelItem);
  Q_DISABLE_COPY(voDataModelItem);
};

#endif
