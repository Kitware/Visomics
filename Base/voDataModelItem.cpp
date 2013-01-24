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
#include <QList>
#include <QSharedPointer>
#include <QUuid>
#include <QDebug>

// Visomics includes
#include "voApplication.h"
#include "voDataModelItem.h"
#include "voDataModel.h"
#include "voDataObject.h"
#include "voInputFileDataObject.h"
#include "voTableDataObject.h"
#include "voView.h"

// --------------------------------------------------------------------------
class voDataModelItemPrivate
{
  Q_DECLARE_PUBLIC(voDataModelItem);
protected:
  voDataModelItem* const q_ptr;
public:
  voDataModelItemPrivate(voDataModelItem& object);

  QExplicitlySharedDataPointer<voDataObject>  DataObject;
  QList<voDataModelItem*>                     ChildItems;
  int                                         Column;
  int                                         Type;
};

// --------------------------------------------------------------------------
// voDataModelItemPrivate methods

// --------------------------------------------------------------------------
voDataModelItemPrivate::voDataModelItemPrivate(voDataModelItem& object):q_ptr(&object)
{
  Q_Q(voDataModelItem);
  this->Type = voDataModelItem::Unspecified;
  this->Column = -1;
  q->setData(QVariant(QUuid::createUuid().toString()), voDataModelItem::UuidRole);
}

// --------------------------------------------------------------------------
// voDataModelItem methods

// --------------------------------------------------------------------------
voDataModelItem::voDataModelItem(voDataObject * newDataObject, int newColumn):
    d_ptr(new voDataModelItemPrivate(*this))
{
  Q_D(voDataModelItem);

  if (!newDataObject)
    {
    qWarning() << "Failed to initialized voDataModelItem - DataObject is NULL";
    return;
    }

  d->DataObject = QExplicitlySharedDataPointer<voDataObject>(newDataObject);
  d->Column = newColumn;

  Qt::ItemFlags flags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

  switch(d->Column)
    {
    case voDataModel::NameColumn:
      this->setFlags(flags | /*Qt::ItemIsEditable |*/ Qt::ItemIsSelectable);
      QString uniqueText = d->DataObject->name();
      voApplication *app = voApplication::application();
      if (app)
        {
        voDataModel * model = voApplication::application()->dataModel();
        uniqueText = model->generateUniqueName(d->DataObject->name());
        }
      this->setText(uniqueText);
      break;
    }

  this->setData(QVariant(newDataObject->uuid()), voDataModelItem::UuidRole);
  this->setType(voDataModelItem::typeFromDataObject(newDataObject));
}

// --------------------------------------------------------------------------
voDataModelItem::voDataModelItem(const QString& newText, int newColumn):
    d_ptr(new voDataModelItemPrivate(*this))
{
  Q_D(voDataModelItem);

  d->Column = newColumn;

  Qt::ItemFlags flags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable |
                      Qt::ItemIsSelectable);

  if (d->Column == voDataModel::NameColumn)
    {
      QString uniqueText = newText;
      voApplication *app = voApplication::application();
      if (app)
        {
        voDataModel * model = voApplication::application()->dataModel();
        uniqueText = model->generateUniqueName(newText);
        }
      this->setText(uniqueText);
    }
  this->setFlags(flags);
}

// --------------------------------------------------------------------------
voDataModelItem::~voDataModelItem()
{

}

// --------------------------------------------------------------------------
QString voDataModelItem::viewType()const
{
  return this->data(voDataModelItem::ViewTypeRole).toString();
}

// --------------------------------------------------------------------------
void voDataModelItem::setViewType(const QString& newViewType)
{
  this->setData(QVariant(newViewType), voDataModelItem::ViewTypeRole);
}

// --------------------------------------------------------------------------
QString voDataModelItem::rawViewType()const
{
  return this->data(voDataModelItem::RawViewTypeRole).toString();
}

// --------------------------------------------------------------------------
void voDataModelItem::setRawViewType(const QString& newRawViewType)
{
  this->setData(QVariant(newRawViewType), voDataModelItem::RawViewTypeRole);
}

// --------------------------------------------------------------------------
QString voDataModelItem::uuid() const
{
  return this->data(voDataModelItem::UuidRole).toString();
}

// --------------------------------------------------------------------------
int voDataModelItem::type() const
{
  Q_D(const voDataModelItem);
  return d->Type;
}

// --------------------------------------------------------------------------
void voDataModelItem::setType(int newType)
{
  Q_D(voDataModelItem);
  d->Type = newType;
}

// --------------------------------------------------------------------------
int voDataModelItem::typeFromDataObject(voDataObject * newDataObject)
{
  if (!newDataObject)
    {
    return voDataModelItem::Unspecified;
    }
  else if (dynamic_cast<voInputFileDataObject*>(newDataObject))
    {
    return voDataModelItem::InputType;
    }
  else if (dynamic_cast<voTableDataObject*>(newDataObject))
    {
    return voDataModelItem::OutputType;
    }
  return voDataModelItem::Unspecified;
}

// --------------------------------------------------------------------------
QVariant voDataModelItem::data(int role) const
{
  if (role == Qt::DecorationRole)
    {
    if (this->type() == voDataModelItem::ContainerType)
      {
      return QIcon(":/Icons/container.png");
      }
    else if (this->type() == voDataModelItem::InputType)
      {
      return QIcon(":/Icons/table.png");
      }
    else if (this->type() == voDataModelItem::OutputType)
      {
      return QIcon(":/Icons/table.png");
      }
    else if (this->type() == voDataModelItem::ViewType)
      {
      return QIcon(":/Icons/graph.png");
      }
    }
  return this->Superclass::data(role);
}

// --------------------------------------------------------------------------
void voDataModelItem::setDataObject(voDataObject* newDataObject)
{
  Q_D(voDataModelItem);
  d->DataObject = QExplicitlySharedDataPointer<voDataObject>(newDataObject);

  if (this->type() == voDataModelItem::InputType ||
      this->type() == voDataModelItem::OutputType ||
      this->type() == voDataModelItem::ViewType)
    {
    voView * view =
        reinterpret_cast<voView*>(this->data(voDataModelItem::ViewVoidStarRole).value<void*>());
    if (view)
      {
      view->setDataObject(newDataObject);
      }
    }
}

// --------------------------------------------------------------------------
voDataObject * voDataModelItem::dataObject() const
{
  Q_D(const voDataModelItem);
  return d->DataObject.data();
}

// --------------------------------------------------------------------------
QList<voDataModelItem*> voDataModelItem::childItems() const
{
  Q_D(const voDataModelItem);
  return d->ChildItems;
}

// --------------------------------------------------------------------------
void voDataModelItem::addChildItem(voDataModelItem * childItem)
{
  Q_D(voDataModelItem);
  d->ChildItems.append(childItem);
}

// --------------------------------------------------------------------------
void voDataModelItem::removeChildItem(voDataModelItem * childItem)
{
  Q_D(voDataModelItem);
  d->ChildItems.removeOne(childItem);
}
