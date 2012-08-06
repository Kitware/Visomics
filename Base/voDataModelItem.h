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
    ViewVoidStarRole,
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
