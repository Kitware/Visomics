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
#ifndef __voMongoSaveDialog_h
#define __voMongoSaveDialog_h

// Qt includes
#include <QDialog>

class voRemoteAnalysisConnectionDialogPrivate;

class voRemoteAnalysisConnectionDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;

  voRemoteAnalysisConnectionDialog(QWidget* newParent = 0);
  virtual ~voRemoteAnalysisConnectionDialog();

  QString Url();
  QString User();
  QString Password();


private:
  QScopedPointer<voRemoteAnalysisConnectionDialogPrivate> d_ptr;
  Q_DECLARE_PRIVATE(voRemoteAnalysisConnectionDialog);
  Q_DISABLE_COPY(voRemoteAnalysisConnectionDialog);

  void loadSettings();

private slots:
  void saveSettings();
};

#endif
