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

#ifndef __voDelimitedTextImportWidget_h
#define __voDelimitedTextImportWidget_h

// Qt includes
#include <QWidget>

class voDelimitedTextPreviewModel;
class voDelimitedTextImportWidgetPrivate;

class voDelimitedTextImportWidget : public QWidget
{
  Q_OBJECT
public:
  typedef voDelimitedTextImportWidget Self;
  typedef QWidget Superclass;

  voDelimitedTextImportWidget(QWidget* newParent = 0);
  virtual ~voDelimitedTextImportWidget();

  enum InsertWidgetLocation
    {
    DelimiterGroupBox = 0,
    RowsAndColumnsGroupBox,
    };

  void insertWidget(QWidget * widget, InsertWidgetLocation location);

  voDelimitedTextPreviewModel* delimitedTextPreviewModel();
  void setDelimitedTextPreviewModel(voDelimitedTextPreviewModel* model);

protected slots:
  void onNumberOfColumnMetaDataTypesChanged(int value);
  void onColumnMetaDataTypeOfInterestChanged(int value);
  void onNumberOfRowMetaDataTypesChanged(int value);
  void onRowMetaDataTypeOfInterestChanged(int value);

  void onDelimiterChanged(int delimiter);
  void onOtherDelimiterLineEditChanged(const QString& text);

  void onStringDelimiterEnabled(bool value);
  void onStringDelimiterLineEditChanged(const QString& text);

protected:
  QScopedPointer<voDelimitedTextImportWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDelimitedTextImportWidget);
  Q_DISABLE_COPY(voDelimitedTextImportWidget);
};

#endif

