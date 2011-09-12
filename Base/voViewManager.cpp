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

// Visomics includes
#include "voApplication.h"
#include "voDataModelItem.h"
#include "voDataObject.h"
#include "voView.h"
#include "voViewFactory.h"
#include "voViewManager.h"


// --------------------------------------------------------------------------
class voViewManagerPrivate
{
public:
  voViewManagerPrivate();

  QHash<QString, voView*> UuidToViewMap;

};

// --------------------------------------------------------------------------
// voViewManagerPrivate methods

// --------------------------------------------------------------------------
voViewManagerPrivate::voViewManagerPrivate()
{
}

// --------------------------------------------------------------------------
// voViewManager methods

// --------------------------------------------------------------------------
voViewManager::voViewManager():d_ptr(new voViewManagerPrivate)
{
}

// --------------------------------------------------------------------------
voViewManager::~voViewManager()
{
}

// --------------------------------------------------------------------------
void voViewManager::createView(const QString& objectUuid)
{
  Q_D(voViewManager);

  if (QUuid(objectUuid).isNull())
    {
    qCritical() << "voViewManager - Failed to create view: objectUuid is NULL";
    return;
    }

  voDataModel * dataModel = voApplication::application()->dataModel();

  voDataModelItem* dataModelItem = dataModel->findItemWithUuid(objectUuid);
  if (!dataModelItem)
    {
    qCritical() << "voViewManager - Failed to create view: dataModelItem is NULL";
    return;
    }

  QString viewType = dataModelItem->viewType();
  if (dataModelItem->type() == voDataModelItem::InputType ||
      dataModelItem->type() == voDataModelItem::OutputType)
    {
    viewType = dataModelItem->rawViewType();
    }
  voDataObject* dataObject = dataModelItem->dataObject();

  if (viewType.isEmpty())
    {
    qCritical() << "voViewManager - Failed to create view: viewType is an empty string";
    return;
    }
  if (!dataObject)
    {
    qCritical() << "voViewManager - Failed to create view: dataObject is NULL";
    return;
    }
  // Check if view has already been instantiated
  voView * view = 0;
  if (d->UuidToViewMap.contains(objectUuid))
    {
    view = d->UuidToViewMap.value(objectUuid);
    Q_ASSERT(view);
    }
  else
    {
    view = voApplication::application()->viewFactory()->createView(viewType);
    if (!view)
      {
      qCritical() << "voViewManager - Failed to instantiate view" << viewType;
      return;
      }
    QString analysisName = dataModel->analysisNameForUuid(objectUuid);
    QString viewName = QString("%1 / %2").arg(analysisName).arg(dataModelItem->text());
    view->setObjectName(viewName);
    d->UuidToViewMap.insert(objectUuid, view);
    }

  Q_ASSERT(viewType == view->metaObject()->className());

  // Associate view with the dataModelItem
  if (dataModelItem->type() == voDataModelItem::InputType ||
      dataModelItem->type() == voDataModelItem::OutputType ||
      dataModelItem->type() == voDataModelItem::ViewType)
    {
    dataModelItem->setData(QVariant(QMetaType::VoidStar, &view), voDataModelItem::ViewVoidStarRole);
    }

  view->setDataObject(dataObject);

  emit this->viewCreated(objectUuid, view);
}

// --------------------------------------------------------------------------
void voViewManager::deleteView(voView * view)
{
  Q_D(voViewManager);
  Q_ASSERT(view);
  if (!view)
    {
    return;
    }

  QString uuidFound;

  // Retrieve uuid associated with the view
  foreach(const QString& uuid, d->UuidToViewMap.keys())
    {
    if (d->UuidToViewMap.value(uuid) == view)
      {
      uuidFound = uuid;
      break;
      }
    }

  // Delete view
  delete view;

  // Remove map entry
  d->UuidToViewMap.remove(uuidFound);
}
