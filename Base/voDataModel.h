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
  voDataModelItem* selectedObject() const;

  bool removeObject(voDataModelItem *itemToRemove, QStandardItem* parent = 0);

  voAnalysis* activeAnalysis()const;
  voDataModelItem* inputTargetForAnalysis(voAnalysis * analysis)const;
  voDataModelItem* itemForAnalysis(voAnalysis * analysis)const;
  voAnalysis* analysisAboveItem(QStandardItem* item)const;

  voDataModelItem* findItemWithUuid(const QString& uuid)const;
  QList<voDataModelItem*> findItemsWithRole(int role, const QVariant& value, voDataModelItem * start = 0)const;
  voDataModelItem* findItemWithText(const QString& text,
                                    QStandardItem* parent = 0) const;

  QString getNextName(const QString& name);

  QString analysisNameForUuid(const QString& uuid);

  QString generateUniqueName(QString desiredName);

  void listItems(const QString &type, QStringList *list, QStandardItem* parent = 0)const;

  const QList<voDataModelItem*>& analyses() const;

public slots:
  void setActiveAnalysis(voAnalysis* analysis);

signals:

  void viewSelected(const QString& objectUuid);
  void inputSelected(QList<voDataModelItem*> inputTargets);

  void activeAnalysisChanged(voAnalysis * analysis);
  void analysisSelected(voAnalysis * analysis);

  void objectRemoved(const QString& objectUuid);

protected:
  bool nameIsAvailable(QString desiredName, QStandardItem* parent);
  void removeChildren(voDataModelItem *parent);

  QScopedPointer<voDataModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDataModel);
  Q_DISABLE_COPY(voDataModel);

};

#endif
