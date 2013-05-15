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
#ifndef __voOpenTreeLoadDialog_h
#define __voOpenTreeLoadDialog_h

// Qt includes
#include <QDialog>

class voOpenTreeLoadDialogPrivate;

class voOpenTreeLoadDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;

  voOpenTreeLoadDialog(QWidget* newParent = 0);
  virtual ~voOpenTreeLoadDialog();

  const QString GetHostURL();
  const QString GetDatabaseName();
  const QString GetOttolID();
  const QString GetMaxDepth();

protected:
  QScopedPointer<voOpenTreeLoadDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voOpenTreeLoadDialog);
  Q_DISABLE_COPY(voOpenTreeLoadDialog);
};

#endif
