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


#ifndef __voView_h
#define __voView_h

// Qt includes
#include <QScopedPointer>
#include <QWidget>

class voDataObject;
class voViewPrivate;

class voView : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  voView(QWidget* newParent = 0);
  virtual ~voView();

  virtual void initialize();

  virtual QString hints()const;

  voDataObject* dataObject()const;

  void setDataObject(voDataObject* dataObject);

  virtual QList<QAction*> actions();

  void saveScreenshot(const QString& fileName);
  void saveScreenshot(const QString& fileName, const QSize& size);

protected slots:
  void onSaveScreenshotActionTriggered();

protected:
  virtual void setupUi(QLayout * layout) = 0;

  virtual QWidget* mainWidget();

  virtual void setDataObjectInternal(const voDataObject& dataObject) = 0;

protected:
  QScopedPointer<voViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voView);
  Q_DISABLE_COPY(voView);
};

#endif
