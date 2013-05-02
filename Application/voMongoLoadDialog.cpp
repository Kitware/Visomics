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
#include "ui_voMongoLoadDialog.h"
#include "voMongoLoadDialog.h"

// Qt includes
#include <QComboBox>

class voMongoLoadDialogPrivate : public Ui_voMongoLoadDialog
{

public:
  typedef Ui_voMongoLoadDialog Superclass;
  void setupUi(QDialog *widget);
};

// --------------------------------------------------------------------------
// voMongoLoadDialogPrivate methods


// --------------------------------------------------------------------------
void voMongoLoadDialogPrivate::setupUi(QDialog *widget)
{
  this->Superclass::setupUi(widget);
}

// --------------------------------------------------------------------------
// voMongoLoadDialog methods

// --------------------------------------------------------------------------
voMongoLoadDialog::voMongoLoadDialog(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voMongoLoadDialogPrivate())
{
  Q_D(voMongoLoadDialog);
  d->setupUi(this);
  this->disableOkButton();
}

// --------------------------------------------------------------------------
voMongoLoadDialog::~voMongoLoadDialog()
{
}

// --------------------------------------------------------------------------
const QString voMongoLoadDialog::GetHost()
{
  Q_D(voMongoLoadDialog);
  return d->hostInput->text();
}

// --------------------------------------------------------------------------
const QString voMongoLoadDialog::GetDatabase()
{
  Q_D(voMongoLoadDialog);
  return d->databaseInput->text();
}

// --------------------------------------------------------------------------
const QString voMongoLoadDialog::GetCollection()
{
  Q_D(voMongoLoadDialog);
  return d->collectionInput->text();
}

// --------------------------------------------------------------------------
const QString voMongoLoadDialog::GetWorkflow()
{
  Q_D(voMongoLoadDialog);
  return d->workflowBox->currentText();
}

// --------------------------------------------------------------------------
QPushButton* voMongoLoadDialog::connectButton()
{
  Q_D(voMongoLoadDialog);
  return d->connectButton;
}

// --------------------------------------------------------------------------
QComboBox* voMongoLoadDialog::workflowBox()
{
  Q_D(voMongoLoadDialog);
  return d->workflowBox;
}

// --------------------------------------------------------------------------
void voMongoLoadDialog::enableOkButton()
{
  Q_D(voMongoLoadDialog);
  d->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

// --------------------------------------------------------------------------
void voMongoLoadDialog::disableOkButton()
{
  Q_D(voMongoLoadDialog);
  d->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

// --------------------------------------------------------------------------
void voMongoLoadDialog::connectionFailed()
{
  Q_D(voMongoLoadDialog);
  d->workflowBox->clear();
  d->workflowBox->setEnabled(false);
  d->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}
