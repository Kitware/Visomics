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
#include "ui_voRemoteAnalysisConnectionDialog.h"
#include "voRemoteAnalysisConnectionDialog.h"

#include <QtCore/QSettings>

class voRemoteAnalysisConnectionDialogPrivate : public Ui_voRemoteAnalysisConnectionDialog
{
public:
  typedef Ui_voRemoteAnalysisConnectionDialog Superclass;
  void setupUi(QDialog *widget);
};

// --------------------------------------------------------------------------
void voRemoteAnalysisConnectionDialogPrivate::setupUi(QDialog *widget)
{
  this->Superclass::setupUi(widget);
}

// --------------------------------------------------------------------------
// voRemoteAnalysisConnectionDialog methods

// --------------------------------------------------------------------------
voRemoteAnalysisConnectionDialog::voRemoteAnalysisConnectionDialog(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voRemoteAnalysisConnectionDialogPrivate())
{
  Q_D(voRemoteAnalysisConnectionDialog);
  d->setupUi(this);

  loadSettings();

  connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

// --------------------------------------------------------------------------
voRemoteAnalysisConnectionDialog::~voRemoteAnalysisConnectionDialog()
{
}

QString voRemoteAnalysisConnectionDialog::Url()
{
  return d_ptr->url->text();
}

QString voRemoteAnalysisConnectionDialog::User()
{
  return d_ptr->user->text();
}

QString voRemoteAnalysisConnectionDialog::Password()
{
  return d_ptr->password->text();
}

void voRemoteAnalysisConnectionDialog::loadSettings()
{
  Q_D(voRemoteAnalysisConnectionDialog);
  QSettings settings("Kitware", "Visomics");
  d->url->setText(settings.value("url", "http://arbor.kitware.com/service").toString());
  d->user->setText(settings.value("user").toString());
  d->password->setText(settings.value("password").toString());
}

void voRemoteAnalysisConnectionDialog::saveSettings()
{
  Q_D(voRemoteAnalysisConnectionDialog);
  QSettings settings("Kitware", "Visomics");
  settings.setValue("url", d->url->text());
  settings.setValue("user", d->user->text());
  settings.setValue("password", d->password->text());
}
