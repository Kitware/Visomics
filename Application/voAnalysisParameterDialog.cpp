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

// Qt includes
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// QtPropertyBrowser includes
#include <QtGroupBoxPropertyBrowser>
#include <QtVariantEditorFactory>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisParameterDialog.h"
#include "voApplication.h"
#include "voDataModel.h"

class voAnalysisParameterDialogPrivate
{
  Q_DECLARE_PUBLIC(voAnalysisParameterDialog);

protected:
  voAnalysisParameterDialog* const q_ptr;

public:
  voAnalysisParameterDialogPrivate(voAnalysisParameterDialog& object);
  void init();

  QLabel* AnalysisDescriptionLabel;
  QtGroupBoxPropertyBrowser * AnalysisParameterEditor;
};

// --------------------------------------------------------------------------
// voAnalysisParameterDialogPrivate methods

// --------------------------------------------------------------------------
voAnalysisParameterDialogPrivate::voAnalysisParameterDialogPrivate(voAnalysisParameterDialog& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void voAnalysisParameterDialogPrivate::init()
{
  Q_Q(voAnalysisParameterDialog);

  q->setWindowTitle("Run analysis - Parameters");

  QVBoxLayout * verticalLayout = new QVBoxLayout();
  q->setLayout(verticalLayout);

  this->AnalysisDescriptionLabel = new QLabel();
  this->AnalysisDescriptionLabel->setWordWrap(true);
  verticalLayout->addWidget(this->AnalysisDescriptionLabel);

  this->AnalysisParameterEditor = new QtGroupBoxPropertyBrowser(q);
  verticalLayout->addWidget(this->AnalysisParameterEditor);

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  verticalLayout->addWidget(buttonBox);

  buttonBox->button(QDialogButtonBox::Ok)->setText("Run Analysis");

  QObject::connect(buttonBox, SIGNAL(rejected()), q, SLOT(reject()));
  QObject::connect(buttonBox, SIGNAL(accepted()), q, SLOT(accept()));
}

// --------------------------------------------------------------------------
// voAnalysisParameterDialog methods

// --------------------------------------------------------------------------
voAnalysisParameterDialog::voAnalysisParameterDialog(voAnalysis * analysis, QWidget* newParent) :
  Superclass(newParent), d_ptr(new voAnalysisParameterDialogPrivate(*this))
{
  Q_ASSERT(analysis);

  Q_D(voAnalysisParameterDialog);
  d->init();

  d->AnalysisDescriptionLabel->setText(analysis->parameterDescription());

  QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);

  d->AnalysisParameterEditor->clear();
  d->AnalysisParameterEditor->setFactoryForManager(analysis->propertyManager(), variantFactory);

  typedef QPair<QString, QString> DynamicPropertyType;
  foreach(const DynamicPropertyType dynamicProp, analysis->dynamicParameters())
    {
    QString type = dynamicProp.first;
    QString label = dynamicProp.second;
    QStringList choices;
    voApplication::application()->dataModel()->listItems(type, &choices);
    analysis->updateEnumParameter(label, choices);
    }

  foreach(QtProperty* prop, analysis->topLevelParameterGroups())
    {
    d->AnalysisParameterEditor->addProperty(prop);
    }
}

// --------------------------------------------------------------------------
voAnalysisParameterDialog::~voAnalysisParameterDialog()
{
}
