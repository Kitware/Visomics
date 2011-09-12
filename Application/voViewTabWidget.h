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

#ifndef __voViewTabWidget_h
#define __voViewTabWidget_h

// Qt includes
#include <QScopedPointer>
#include <QTabWidget>

class voViewTabWidgetPrivate;
class voAnalysis;
class voView;

class voViewTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  typedef QTabWidget Superclass;
  voViewTabWidget(QWidget * newParent = 0);
  virtual ~voViewTabWidget();

public slots:
  void addView(const QString& objectUuid, voView * newView);

protected slots:

  void onTabCloseRequested(int index);

protected:
  QScopedPointer<voViewTabWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voViewTabWidget);
  Q_DISABLE_COPY(voViewTabWidget);

};

#endif
