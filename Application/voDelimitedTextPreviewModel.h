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

#ifndef __voDelimitedTextPreviewModel_h
#define __voDelimitedTextPreviewModel_h

// Qt includes
#include <QStandardItemModel>
#include <QString>

// Macros - TODO: find a way to import and include ctkPimpl.h

class voDelimitedTextImportSettings;
class voDelimitedTextPreviewModelPrivate;
class vtkTable;

class voDelimitedTextPreviewModel : public QStandardItemModel
{
  Q_OBJECT

public:
  typedef QStandardItemModel Superclass;
  voDelimitedTextPreviewModel(const voDelimitedTextImportSettings& defaultSettings,
                              QObject* newParent = 0);
  virtual ~voDelimitedTextPreviewModel();


  QString fileName() const;

  QString fieldDelimiterCharacters() const;
  void setFieldDelimiter(char delimiter);

  char stringDelimiter() const;
  void setStringDelimiter(char character); // Value of 0 indicates none

  bool transpose() const;

  int numberOfColumnMetaDataTypes() const;
  int columnMetaDataTypeOfInterest() const;

  int numberOfRowMetaDataTypes() const;
  int rowMetaDataTypeOfInterest() const;

  int numberOfRowsToPreview() const;

  bool inlineUpdate() const;

  bool mergeConsecutiveDelimiters()const;

  bool useStringDelimiter()const;

  void resetDataTable();

  vtkTable * dataTable();

public slots:

  void setFileName(const QString& newFileName);

  void setTranspose(bool value);

  void setNumberOfColumnMetaDataTypes(int count);
  void setColumnMetaDataTypeOfInterest(int count);

  void setNumberOfRowMetaDataTypes(int count);
  void setRowMetaDataTypeOfInterest(int count);

  void setNumberOfRowsToPreview(int count);

  void setInlineUpdate(bool value);

  void updatePreview();

signals:
  void fileNameChanged(const QString& filename);

  void fieldDelimiterChanged(char delimiter);

  void inlineUpdateChanged(bool value);

  void numberOfColumnMetaDataTypesChanged(int value);
  void columnMetaDataTypeOfInterestChanged(int value);

  void numberOfRowMetaDataTypesChanged(int value);
  void rowMetaDataTypeOfInterestChanged(int value);

  void numberOfRowsToPreviewChanged(int count);

  void stringDelimiterChanged(char character);

  void transposeChanged(bool transpose);

private:
  QScopedPointer<voDelimitedTextPreviewModelPrivate> d_ptr;

  Q_DECLARE_PRIVATE(voDelimitedTextPreviewModel);
  Q_DISABLE_COPY(voDelimitedTextPreviewModel);
};

#endif
