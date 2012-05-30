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
#ifndef __voDelimitedTextImportDialog_h
#define __voDelimitedTextImportDialog_h

// Qt includes
#include <QDialog>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

class voDelimitedTextImportDialogPrivate;

class voDelimitedTextImportDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;

  voDelimitedTextImportDialog(QWidget* newParent = 0,
                              const voDelimitedTextImportSettings& defaultSettings =
                                voDelimitedTextImportSettings());
  virtual ~voDelimitedTextImportDialog();

  void setFileName(const QString& fileName);

  voDelimitedTextImportSettings importSettings()const;

protected slots:
  void setNormalizationMethod(const QString& normalizationMethodName);

protected:
  QScopedPointer<voDelimitedTextImportDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDelimitedTextImportDialog);
  Q_DISABLE_COPY(voDelimitedTextImportDialog);
};

#endif
