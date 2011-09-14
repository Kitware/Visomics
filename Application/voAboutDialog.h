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

#ifndef __voAboutDialog_h
#define __voAboutDialog_h

// Qt includes
#include <QMessageBox>
#include <QScopedPointer>

class voAboutDialogPrivate;

class voAboutDialog : public QMessageBox
{
  Q_OBJECT

public:
  typedef QMessageBox Superclass;
  voAboutDialog(QWidget * newParent = 0);
  virtual ~voAboutDialog();

protected:
  QScopedPointer<voAboutDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAboutDialog);
  Q_DISABLE_COPY(voAboutDialog);
};

#endif
