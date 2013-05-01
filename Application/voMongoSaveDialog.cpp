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
#include "ui_voMongoSaveDialog.h"
#include "voMongoSaveDialog.h"

class voMongoSaveDialogPrivate : public Ui_voMongoSaveDialog
{

public:
  typedef Ui_voMongoSaveDialog Superclass;
  void setupUi(QDialog *widget);
};

// --------------------------------------------------------------------------
// voMongoSaveDialogPrivate methods


// --------------------------------------------------------------------------
void voMongoSaveDialogPrivate::setupUi(QDialog *widget)
{
  this->Superclass::setupUi(widget);
}

// --------------------------------------------------------------------------
// voMongoSaveDialog methods

// --------------------------------------------------------------------------
voMongoSaveDialog::voMongoSaveDialog(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voMongoSaveDialogPrivate())
{
  Q_D(voMongoSaveDialog);
  d->setupUi(this);
}

// --------------------------------------------------------------------------
voMongoSaveDialog::~voMongoSaveDialog()
{
}

// --------------------------------------------------------------------------
const QString voMongoSaveDialog::GetHost()
{
  Q_D(voMongoSaveDialog);
  return d->hostInput->text();
}

// --------------------------------------------------------------------------
const QString voMongoSaveDialog::GetDatabase()
{
  Q_D(voMongoSaveDialog);
  return d->databaseInput->text();
}

// --------------------------------------------------------------------------
const QString voMongoSaveDialog::GetCollection()
{
  Q_D(voMongoSaveDialog);
  return d->collectionInput->text();
}

// --------------------------------------------------------------------------
const QString voMongoSaveDialog::GetWorkflow()
{
  Q_D(voMongoSaveDialog);
  return d->workflowNameInput->text();
}

