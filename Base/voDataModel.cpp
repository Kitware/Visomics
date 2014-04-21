/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QUuid>
#include <QModelIndex>
#include <QItemSelection>
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
  this->ActiveAnalysis = 0;
}

// --------------------------------------------------------------------------
voDataModelPrivate::~voDataModelPrivate()
{
}

// --------------------------------------------------------------------------
void voDataModelPrivate::onSelectionChanged(const QItemSelection & current,
                                            const QItemSelection & previous)
{
  Q_UNUSED(current);
  Q_UNUSED(previous);
  Q_Q(voDataModel);

  this->SelectedDataObject = 0;

  // For some reason "current" only seems to contain the most recently
  // selected items, not the entire list of selected items.
  // To work around this, we use selectedIndexes() instead.
  QModelIndexList selectedIndices = this->SelectionModel->selectedIndexes();

  // Return early if nothing is selected.
  if (selectedIndices.size() == 0)
    {
    return;
    }

  voDataModelItem * item = dynamic_cast<voDataModelItem*>
    (q->itemFromIndex(selectedIndices.last()));
  Q_ASSERT(item);

  // Clear & repopulate list of selected inputs
  this->SelectedInputDataObjects.clear();
  foreach(const QModelIndex& index, selectedIndices)
    {
    voDataModelItem * selectedItem
        = dynamic_cast<voDataModelItem*>(q->itemFromIndex(index));
    Q_ASSERT(selectedItem);
    if (selectedItem->type() == voDataModelItem::InputType ||
        selectedItem->type() == voDataModelItem::OutputType)
      {
      this->SelectedInputDataObjects << selectedItem;
      }
    }

  voAnalysis * selectedAnalysis = 0;

  this->SelectedDataObject = item;

  if (item->type() == voDataModelItem::InputType)
    {
    qDebug() << "onSelectionChanged - InputType" << item->dataObject()->name();
    emit q->viewSelected(item->uuid());
    }
  else if (this->SelectedInputDataObjects.size() == 0)
    {
    // here we assume that any toplevel item is "InputType"
    voDataModelItem * childItem = item;
    while (childItem->parent() != 0)
      {
      childItem = dynamic_cast<voDataModelItem*>(childItem->parent());
      }
    this->SelectedInputDataObjects << childItem;
    }

  if(item->type() == voDataModelItem::OutputType)
    {
    qDebug() << "onSelectionChanged - OutputType" << item->dataObject()->name();
    emit q->viewSelected(item->uuid());
    }
  else if(item->type() == voDataModelItem::ViewType)
    {
    qDebug() << "onSelectionChanged - viewType" << item->viewType();
    emit q->viewSelected(item->uuid());
    }
  else if(item->type() == voDataModelItem::ContainerType)
    {
    selectedAnalysis =
        reinterpret_cast<voAnalysis*>(item->data(voDataModelItem::AnalysisVoidStarRole).value<void*>());
    qDebug() << "onSelectionChanged - ContainerType" << item->text() << "-" << selectedAnalysis;
    }

  // enable & disable the actions within the Analysis menu based on
  // what is currently selected.
  if (!this->SelectedInputDataObjects.empty())
    {
    emit q->inputSelected(this->SelectedInputDataObjects);
    }

  voAnalysis * activeAnalysis = q->analysisAboveItem(item);
  if (activeAnalysis != this->ActiveAnalysis)
    {
    this->ActiveAnalysis = activeAnalysis;
    emit q->activeAnalysisChanged(this->ActiveAnalysis);
    }
  emit q->analysisSelected(selectedAnalysis);
}

// --------------------------------------------------------------------------
// voDataModel methods

// --------------------------------------------------------------------------
voDataModel::voDataModel():Superclass(), d_ptr(new voDataModelPrivate(*this))
{
  Q_D(voDataModel);
  d->SelectionModel = new QItemSelectionModel(this);
  d->SelectionModel->reset();
  connect(
    d->SelectionModel,
    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
    d,
    SLOT(onSelectionChanged(const QItemSelection &, const QItemSelection &)));

  this->setColumnCount(1);
}

// --------------------------------------------------------------------------
voDataModel::~voDataModel()
{
  Q_D(voDataModel);
  this->clear();
  delete d->SelectionModel;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::addContainer(const QString& containerName,
                                           QStandardItem* parent)
{
  Q_D(voDataModel);
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

  d->Analyses.push_back(containerItem);

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
    newItem->setText(""); // keep this item from stepping on its own toes.
    QString uniqueText = this->generateUniqueName(rawViewPrettyName);
    newItem->setText(uniqueText);
    }

  // change icon if this is not a typical "raw data" view
  if (rawViewType != "voExtendedTableView" && rawViewType != "voTableView" &&
      rawViewType != "voTreeHeatmapView")
    {
    newItem->setType(voDataModelItem::ViewType);
    newItem->setViewType(rawViewType);
    }
  else
    {
    newItem->setRawViewType(rawViewType);
    }

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
voDataModelItem* voDataModel::selectedObject() const
{
  Q_D(const voDataModel);
  return d->SelectedDataObject;
}

// --------------------------------------------------------------------------
const QList<voDataModelItem*>& voDataModel::selectedInputObjects() const
{
  Q_D(const voDataModel);
  return d->SelectedInputDataObjects;
}

// --------------------------------------------------------------------------
const QList<voDataModelItem*>& voDataModel::analyses() const
{
  Q_D(const voDataModel);
  return d->Analyses;
}

// --------------------------------------------------------------------------
bool voDataModel::removeObject(voDataModelItem *objectToRemove,
                               QStandardItem* parent)
{
  if (!parent)
    {
    parent = this->invisibleRootItem();
    }
  for (int row = 0; row < this->rowCount(parent->index()); ++row)
    {
    for (int col = 0; col < this->columnCount(parent->index()); ++col)
      {
      voDataModelItem *item =
        dynamic_cast<voDataModelItem*>(
          this->itemFromIndex(this->index(row, col, parent->index())));
      if (item == objectToRemove)
        {
        // delete the views of this object's children,
        // and remove all child analyses from our list
        this->removeChildren(item);

        emit this->objectRemoved(objectToRemove->uuid());
        this->removeRow(row, parent->index());
        if (parent != this->invisibleRootItem())
          {
          voDataModelItem *parentItem = dynamic_cast<voDataModelItem*>(parent);
          parentItem->removeChildItem(item);

          // if we just removed the parent's last row, we should get rid of the
          // parent too.
          if (this->rowCount(parent->index()) == 0 &&
              parent->type() == voDataModelItem::ContainerType)
            {
            this->removeObject(parentItem, parent->parent());
            }
          }
        return true;
        }
      else if (this->hasChildren(item->index()))
        {
        if (this->removeObject(objectToRemove, item))
          {
          return true;
          }
        }
      }
    }
  return false;
}

// --------------------------------------------------------------------------
void voDataModel::removeChildren(voDataModelItem *parent)
{
  Q_D(voDataModel);

  for (int row = 0; row < this->rowCount(parent->index()); ++row)
    {
    for (int col = 0; col < this->columnCount(parent->index()); ++col)
      {
      voDataModelItem *item =
        dynamic_cast<voDataModelItem*>(
          this->itemFromIndex(this->index(row, col, parent->index())));
      if (item->type() == voDataModelItem::ContainerType)
        {
        d->Analyses.removeAll(item);
        }
      if (this->hasChildren(item->index()))
        {
        this->removeChildren(item);
        }
      emit this->objectRemoved(item->uuid());
      }
    }

  if (parent->type() == voDataModelItem::ContainerType)
    {
    d->Analyses.removeAll(parent);
    }
}

// --------------------------------------------------------------------------
voAnalysis* voDataModel::activeAnalysis()const
{
   Q_D(const voDataModel);
  return d->ActiveAnalysis;
}

// --------------------------------------------------------------------------
void voDataModel::setActiveAnalysis(voAnalysis* analysis)
{
  Q_D(voDataModel);
  if (!analysis)
    {
    return;
    }
  QModelIndex index = this->indexFromItem(this->itemForAnalysis(analysis));
  d->SelectionModel->setCurrentIndex(
        index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
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
  QList<voDataModelItem*> items = this->findItemsWithRole(voDataModelItem::UuidRole, uuid);
  Q_ASSERT(items.count() == 1); // Item should be uniquely identified !
  return items.at(0);
}

// --------------------------------------------------------------------------
QList<voDataModelItem*> voDataModel::findItemsWithRole(int role, const QVariant& value, voDataModelItem * start)const
{
  QModelIndex startIndex = this->index(0, 0, QModelIndex());
  if (start)
    {
    startIndex = this->indexFromItem(start);
    Q_ASSERT(startIndex.isValid());
    }
  QModelIndexList indexes = this->match(
      startIndex, role, value, -1, Qt::MatchExactly | Qt::MatchRecursive);

  QList<voDataModelItem*> items;
  foreach(const QModelIndex& index, indexes)
    {
    voDataModelItem * item
        = dynamic_cast<voDataModelItem*>(this->itemFromIndex(index));
    Q_ASSERT(item);
    items << item;
    }
  return items;
}

// --------------------------------------------------------------------------
voDataModelItem* voDataModel::findItemWithText(const QString& text,
                                               QStandardItem* parent) const
{
  if (text.isEmpty() || text.isNull())
    {
    return NULL;
    }

  if (!parent)
    {
    parent = this->invisibleRootItem();
    }

  voDataModelItem *retval = NULL;

  for (int row = 0; row < this->rowCount(parent->index()); ++row)
    {
    for (int col = 0; col < this->columnCount(parent->index()); ++col)
      {
      voDataModelItem *item =
        dynamic_cast<voDataModelItem*>(
          this->itemFromIndex(this->index(row, col, parent->index())));
      if (item->text() == text)
        {
        return item;
        }
      if (this->hasChildren(item->index()))
        {
        retval = this->findItemWithText(text, item);
        if (retval != NULL)
          {
          return retval;
          }
        }
      }
    }
  return retval;
}

// --------------------------------------------------------------------------
QString voDataModel::getNextName(const QString& name)
{
  Q_D(voDataModel);
  int count = d->NameCountMap.value(name, 1);
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

// --------------------------------------------------------------------------
bool voDataModel::nameIsAvailable(QString desiredName,
                                  QStandardItem* parent)
{
  if (!parent)
    {
    parent = this->invisibleRootItem();
    }
  for (int row = 0; row < this->rowCount(parent->index()); ++row)
    {
    for (int col = 0; col < this->columnCount(parent->index()); ++col)
      {
      voDataModelItem *item =
        dynamic_cast<voDataModelItem*>(
          this->itemFromIndex(this->index(row, col, parent->index())));
      if (item->text() == desiredName)
        {
        return false;
        }
      else if (this->hasChildren(item->index()))
        {
        if (!this->nameIsAvailable(desiredName, item))
          {
          return false;
          }
        }
      }
    }
  return true;
}

// --------------------------------------------------------------------------
QString voDataModel::generateUniqueName(QString desiredName)
{
  QString uniqueName = desiredName;
  bool nameIsUnique = this->nameIsAvailable(desiredName, NULL);
  int itr = 2;

  while (!nameIsUnique)
    {
    uniqueName = QString("%1 %2").arg(desiredName).arg(itr);
    nameIsUnique = this->nameIsAvailable(uniqueName, NULL);
    ++itr;
    }

  return uniqueName;
}

// --------------------------------------------------------------------------
void voDataModel::listItems(const QString &type, QStringList *list,
                            QStandardItem* parent) const
{
  if (!parent)
    {
    parent = this->invisibleRootItem();
    }

  for (int row = 0; row < this->rowCount(parent->index()); ++row)
    {
    for (int col = 0; col < this->columnCount(parent->index()); ++col)
      {
      voDataModelItem *item =
        dynamic_cast<voDataModelItem*>(
        this->itemFromIndex(this->index(row, col, parent->index())));

      if (item->dataObject() && item->dataObject()->type() == type)
        {
        *list << item->text();
        }

      if (this->hasChildren(item->index()))
        {
        this->listItems(type, list, item);
        }
      }
    }
}
