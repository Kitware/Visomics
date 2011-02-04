
#ifndef __voDataModel_h
#define __voDataModel_h

// Qt includes
#include <QStandardItemModel>
#include <QScopedPointer>

class QItemSelectionModel;
class voAnalysis;
class voDataModelPrivate;
class voDataModelItem;
class voDataObject;

class voDataModel : public QStandardItemModel
{
  Q_OBJECT
public:
  typedef QStandardItemModel Superclass;
  voDataModel();
  virtual ~voDataModel();

  enum ItemDataRole{
    UIDRole = Qt::UserRole + 1,
    PointerRole
    };

  enum ModelColumn{
    NameColumn = 0,
    };

  voDataModelItem* addContainer(const QString& containerName, QStandardItem* parent = 0);
  voDataModelItem* addView(const QString& viewType, const QString& viewPrettyName,
                           voDataObject * newDataObject, QStandardItem* parent = 0);
  voDataModelItem* addOutput(voDataObject * newDataObject, QStandardItem * parent,
                            const QString& rawViewType = QString(),
                            const QString& rawViewPrettyName = QString());
  voDataModelItem* addDataObject(voDataObject * newDataObject);
  voDataModelItem* addDataObjectAsChild(voDataObject * newDataObject, QStandardItem * parent);

  QItemSelectionModel * selectionModel()const;
  void setSelected(voDataModelItem* itemToSelect);

  voDataModelItem* selectedInputObject() const;
  const QList<voDataModelItem*>& selectedInputObjects() const;

  voDataModelItem* inputTargetForAnalysis(voAnalysis * analysis)const;
  voDataModelItem* itemForAnalysis(voAnalysis * analysis)const;
  voAnalysis* analysisAboveItem(QStandardItem* item)const;

  voDataModelItem* findItemWithUuid(const QString& uuid)const;
  QList<voDataModelItem*> findItemsWithRole(int role, const QVariant& value, voDataModelItem * start = 0)const;

  QString getNextName(const QString& name);

  QString analysisNameForUuid(const QString& uuid);

signals:

  void viewSelected(const QString& objectUuid);

protected:
  QScopedPointer<voDataModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDataModel);
  Q_DISABLE_COPY(voDataModel);
  
};

#endif
