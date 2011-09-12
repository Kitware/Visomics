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

#ifndef __voNormalizationWidget_h
#define __voNormalizationWidget_h

// Qt includes
#include <QWidget>

class voNormalizationWidgetPrivate;

class voNormalizationWidget : public QWidget
{
  Q_OBJECT
public:
  typedef voNormalizationWidget Self;
  typedef QWidget Superclass;

  voNormalizationWidget(QWidget* newParent = 0);
  virtual ~voNormalizationWidget();

  QString selectedNormalizationMethod()const;

signals:
  void normalizationMethodSelected(const QString& methodName);

protected slots:
  void selectNormalizationMethod(const QString& methodName);

protected:
  QScopedPointer<voNormalizationWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voNormalizationWidget);
  Q_DISABLE_COPY(voNormalizationWidget);
};

#endif

