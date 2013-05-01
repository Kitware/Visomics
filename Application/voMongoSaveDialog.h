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

class voMongoSaveDialogPrivate;

class voMongoSaveDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;

  voMongoSaveDialog(QWidget* newParent = 0);
  virtual ~voMongoSaveDialog();

  const QString GetHost();
  const QString GetDatabase();
  const QString GetCollection();
  const QString GetWorkflow();

protected:
  QScopedPointer<voMongoSaveDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voMongoSaveDialog);
  Q_DISABLE_COPY(voMongoSaveDialog);
};

#endif
