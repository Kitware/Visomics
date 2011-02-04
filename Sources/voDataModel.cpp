
// Qt includes
#include <QDebug>
#include <QUuid>
#include <QItemSelectionModel>

// Visomics includes
#include "voAnalysis.h"
#include "voDataModel.h"
#include "voDataModel_p.h"
#include "voDataModelItem.h"
#include "voDataObject.h"

// --------------------------------------------------------------------------
// voDataModelPrivate methods

// --------------------------------------------------------------------------
voDataModelPrivate::voDataModelPrivate(voDataModel& object) : q_ptr(&object)
{
  this->SelectionModel = 0;
}

// --------------------------------------------------------------------------
voDataModelPrivate::~voDataModelPrivate()
{
}

// --------------------------------------------------------------------------
void voDataModelPrivate::onCurrentRowChanged(const QModelIndex & current,
                                             const QModelIndex & previous)
{
  Q_UNUSED(previous);
  Q_Q(voDataModel);
  voDataModelItem * item = dynamic_cast<voDataModelItem*>(q->itemFromIndex(current));
  Q_ASSERT(item);
  if (!item)
    {
    return;
    }

  // Clear list
  this->SelectedInputDataObjects.clear();

  if (item->type() == voDataModelItem::InputType)
    {
    qDebug() << "onCurrentRowChanged - InputType" << item->dataObject()->name();
    this->SelectedInputDataObjects << item;
    emit q->viewSelected(item->uuid());
    }
  else if(item->type() == voDataModelItem::OutputType)
    {
    qDebug() << "onCurrentRowChanged - OutputType" << item->dataObject()->name();
    emit q->viewSelected(item->uuid());
    }
  else if(item->type() == voDataModelItem::ViewType)
    {
    qDebug() << "onCurrentRowChanged - viewType" << item->viewType();
    emit q->viewSelected(item->uuid());
    }
  else if(item->type() == voDataModelItem::ContainerType)
    {
    qDebug() << "onCurrentRowChanged - ContainerType" << item->text();
    }
}

// --------------------------------------------------------------------------
// voDataModel methods

// --------------------------------------------------------------------------
voDataModel::voDataModel():Superclass(), d_ptr(new voDataModelPrivate(*this))
{
  Q_D(voDataModel);
  d->SelectionModel = new QItemSelectionModel(this);
  d->SelectionModel->reset();
  connect(d->SelectionModel, SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
          d, SLOT(onCurrentRowChanged(const QModelIndex &, const QModelIndex &)));

  this->setColumnCount(1);
}

// --------------------------------------------------------------------------
voDataModel::~voDataModel()
{
  Q_D(voDataModel);
  delete d->SelectionModel;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::addContainer(const QString& containerName,
                                           QStandardItem* parent)
{
  Q_ASSERT(!containerName.isEmpty());
  if (containerName.isEmpty())
    {
    return 0;
    }

  voDataModelItem * containerItem = new voDataModelItem(containerName);
  containerItem->setType(voDataModelItem::ContainerType);

  if (!parent)
    {
    parent = this->invisibleRootItem();
    }

  parent->appendRow(containerItem);

  return containerItem;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::addView(const QString& viewType, const QString& viewPrettyName,
                                      voDataObject * newDataObject, QStandardItem* parent)
{
  Q_ASSERT(!viewType.isEmpty());
  if (viewType.isEmpty())
    {
    return 0;
    }

  QString tmpViewPrettyName = viewPrettyName;
  if (tmpViewPrettyName.isEmpty())
    {
    tmpViewPrettyName = viewType;
    }

  voDataModelItem * viewItem = new voDataModelItem(tmpViewPrettyName);
  viewItem->setType(voDataModelItem::ViewType);
  viewItem->setViewType(viewType);
  viewItem->setDataObject(newDataObject);

  if (!parent)
    {
    parent = this->invisibleRootItem();
    }

  parent->appendRow(viewItem);

  return viewItem;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::addOutput(voDataObject * newDataObject, QStandardItem * parent,
                                       const QString& rawViewType, const QString& rawViewPrettyName)
{
  voDataModelItem * newItem = this->addDataObjectAsChild(newDataObject, parent);
  if (!rawViewPrettyName.isEmpty())
    {
    newItem->setText(rawViewPrettyName);
    }
  newItem->setRawViewType(rawViewType);
  return newItem;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::addDataObject(voDataObject * newDataObject)
{
  return this->addDataObjectAsChild(newDataObject, this->invisibleRootItem());
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::addDataObjectAsChild(
    voDataObject * newDataObject, QStandardItem * parent)
{
  Q_D(voDataModel);

  QList<QStandardItem*> items;
  for (int i= 0; i < this->columnCount(); ++i)
    {
    voDataModelItem* newDataModelItem = new voDataModelItem(newDataObject, i);
    items.append(newDataModelItem);
    }
  if (parent)
    {
    parent->appendRow(items);
    }
  else
    {
    this->appendRow(items);
    }

  return dynamic_cast<voDataModelItem*>(items.value(0));
}

// --------------------------------------------------------------------------
QItemSelectionModel * voDataModel::selectionModel()const
{
  Q_D(const voDataModel);
  return d->SelectionModel;
}

// --------------------------------------------------------------------------
void voDataModel::setSelected(voDataModelItem* itemToSelect)
{
  Q_D(voDataModel);

  if (!itemToSelect)
    {
    return;
    }

  // Set current index
  d->SelectionModel->setCurrentIndex(
      this->indexFromItem(itemToSelect),
      QItemSelectionModel::Rows | QItemSelectionModel::Current | QItemSelectionModel::ClearAndSelect);
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::selectedInputObject() const
{
  Q_D(const voDataModel);
  if (d->SelectedInputDataObjects.count() > 0)
    {
    return d->SelectedInputDataObjects.value(0);
    }
  return 0;
}

// --------------------------------------------------------------------------
const QList<voDataModelItem*>& voDataModel::selectedInputObjects() const
{
  Q_D(const voDataModel);
  return d->SelectedInputDataObjects;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::inputTargetForAnalysis(voAnalysis * analysis)const
{
  voDataModelItem * analysisItem = this->itemForAnalysis(analysis);
  if (!analysisItem)
    {
    return 0;
    }
  QStandardItem * item = analysisItem->parent();
  while(item)
    {
    if (item->type() == voDataModelItem::InputType)
      {
      return dynamic_cast<voDataModelItem*>(item);
      }
    item = item->parent();
    }
  return 0;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::itemForAnalysis(voAnalysis * analysis)const
{
  if (!analysis)
    {
    return 0;
    }
  return this->findItemWithUuid(analysis->uuid());
}

// --------------------------------------------------------------------------
voAnalysis* voDataModel::analysisAboveItem(QStandardItem* item)const
{
  if (!item)
    {
    return 0;
    }
  if (item->data(voDataModelItem::IsAnalysisContainerRole).toBool())
    {
    return reinterpret_cast<voAnalysis*>(
          item->data(voDataModelItem::AnalysisVoidStarRole).value<void*>());
    }
  else
    {
    return this->analysisAboveItem(item->parent());
    }
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::findItemWithUuid(const QString& uuid)const
{
  if (QUuid(uuid).isNull())
    {
    return 0;
    }

  voDataModelItem* foundItem = 0;

  QModelIndexList indexes = this->match(
      this->index(0, 0, QModelIndex()),
      voDataModelItem::UuidRole, uuid, -1, Qt::MatchExactly | Qt::MatchRecursive);
  if (indexes.count() > 0)
    {
    Q_ASSERT(indexes.count() == 1); // Item should uniquely identified !
    foundItem = dynamic_cast<voDataModelItem*>(this->itemFromIndex(indexes.value(0)));
    Q_ASSERT(foundItem);
    }
  return foundItem;
}

// --------------------------------------------------------------------------
QString voDataModel::getNextName(const QString& name)
{
  Q_D(voDataModel);
  int count = d->NameCountMap.value(name, 0);
  d->NameCountMap.insert(name, count + 1);
  return QString("%1 %2").arg(name).arg(count);
}

// --------------------------------------------------------------------------
QString voDataModel::analysisNameForUuid(const QString& uuid)
{
  voDataModelItem * itemForUuid = this->findItemWithUuid(uuid);
  if (!itemForUuid)
    {
    return 0;
    }
  QModelIndex index = this->indexFromItem(itemForUuid);
  Q_ASSERT(index.isValid());

  QModelIndex parent = index.parent();
  while(parent.isValid())
    {
    QStandardItem* item = this->itemFromIndex(parent);
    Q_ASSERT(item);
    if (item->data(voDataModelItem::IsAnalysisContainerRole).toBool())
      {
      return item->text();
      }
    index = parent;
    parent = index.parent();
    }
  return QString();
}
