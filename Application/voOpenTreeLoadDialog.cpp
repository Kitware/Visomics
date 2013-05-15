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

// Visomics includes
#include "ui_voOpenTreeLoadDialog.h"
#include "voOpenTreeLoadDialog.h"

class voOpenTreeLoadDialogPrivate : public Ui_voOpenTreeLoadDialog
{

public:
  typedef Ui_voOpenTreeLoadDialog Superclass;
  void setupUi(QDialog *widget);
};

// --------------------------------------------------------------------------
// voOpenTreeLoadDialogPrivate methods


// --------------------------------------------------------------------------
void voOpenTreeLoadDialogPrivate::setupUi(QDialog *widget)
{
  this->Superclass::setupUi(widget);
}

// --------------------------------------------------------------------------
// voOpenTreeLoadDialog methods

// --------------------------------------------------------------------------
voOpenTreeLoadDialog::voOpenTreeLoadDialog(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voOpenTreeLoadDialogPrivate())
{
  Q_D(voOpenTreeLoadDialog);
  d->setupUi(this);
}

// --------------------------------------------------------------------------
voOpenTreeLoadDialog::~voOpenTreeLoadDialog()
{
}

// --------------------------------------------------------------------------
const QString voOpenTreeLoadDialog::GetHostURL()
{
  Q_D(voOpenTreeLoadDialog);
  return d->hostURL->text();
}

// --------------------------------------------------------------------------
const QString voOpenTreeLoadDialog::GetDatabaseName()
{
  Q_D(voOpenTreeLoadDialog);
  return d->databaseName->text();
}

// --------------------------------------------------------------------------
const QString voOpenTreeLoadDialog::GetOttolID()
{
  Q_D(voOpenTreeLoadDialog);
  return d->ottolID->text();
}


// --------------------------------------------------------------------------
const QString voOpenTreeLoadDialog::GetMaxDepth()
{
  Q_D(voOpenTreeLoadDialog);
  return d->maxDepth->text();
}

