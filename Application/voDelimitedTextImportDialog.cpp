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
#include <QGroupBox>
#include <QLabel>

// Visomics includes
#include "ui_voDelimitedTextImportDialog.h"
#include "voApplication.h"
#include "voDelimitedTextImportDialog.h"
#include "voDelimitedTextImportWidget.h"
#include "voDelimitedTextPreviewModel.h"
#include "voRegistry.h"

class voDelimitedTextImportDialogPrivate : public Ui_voDelimitedTextImportDialog
{

public:
  typedef Ui_voDelimitedTextImportDialog Superclass;

  voDelimitedTextImportDialogPrivate(const voDelimitedTextImportSettings& defaultSettings);

  QString preProcessingAndFilteringGroupBoxText(const QString& normalizationMethodName);

  void setupUi(QDialog *widget);

  QLabel * NormalizationMethodLabel;

  voDelimitedTextPreviewModel DelimitedTextPreviewModel;
};

// --------------------------------------------------------------------------
// voDelimitedTextImportDialogPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextImportDialogPrivate::voDelimitedTextImportDialogPrivate(
    const voDelimitedTextImportSettings& defaultSettings) :
    DelimitedTextPreviewModel(defaultSettings)
{
  this->NormalizationMethodLabel = 0;
}

// --------------------------------------------------------------------------
QString voDelimitedTextImportDialogPrivate::preProcessingAndFilteringGroupBoxText(
  const QString& normalizationMethodName)
{
  return QString("<ul><li>%1 normalization method is selected. "
      "Click on <i>Normalization</i> tab for more options.</li></ul>").arg(normalizationMethodName);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportDialogPrivate::setupUi(QDialog *widget)
{
  this->Superclass::setupUi(widget);

  this->DelimitedTextImportWidget->setDelimitedTextPreviewModel(&this->DelimitedTextPreviewModel);

  // Add "PreProcessing&Filtering" groupbox to DelimitedTextImportWidget
  QGroupBox * preProcessingAndFilteringGroupBox = new QGroupBox("Pre-processing && Filtering");
  QVBoxLayout * layout = new QVBoxLayout(preProcessingAndFilteringGroupBox);
  this->NormalizationMethodLabel = new QLabel(this->preProcessingAndFilteringGroupBoxText("No"));
  layout->addWidget(this->NormalizationMethodLabel);
  this->DelimitedTextImportWidget->insertWidget(
        preProcessingAndFilteringGroupBox, voDelimitedTextImportWidget::RowsAndColumnsGroupBox);

  //
  QObject::connect(this->NormalizationWidget, SIGNAL(normalizationMethodSelected(const QString&)),
                   widget, SLOT(setNormalizationMethod(const QString&)));

  this->ImportStepTabWidget->setTabEnabled(2, false);
}

// --------------------------------------------------------------------------
// voDelimitedTextImportDialog methods

// --------------------------------------------------------------------------
voDelimitedTextImportDialog::voDelimitedTextImportDialog(
    QWidget* newParent,const voDelimitedTextImportSettings& defaultSettings) :
  Superclass(newParent), d_ptr(new voDelimitedTextImportDialogPrivate(defaultSettings))
{
  Q_D(voDelimitedTextImportDialog);
  d->setupUi(this);

  d->DocumentPreviewWidget->setModel(&d->DelimitedTextPreviewModel);

  d->NormalizationWidget->setSelectedNormalizationMethod(
      defaultSettings.value(voDelimitedTextImportSettings::NormalizationMethod).toString());
}

// --------------------------------------------------------------------------
voDelimitedTextImportDialog::~voDelimitedTextImportDialog()
{
}

// --------------------------------------------------------------------------
void voDelimitedTextImportDialog::setFileName(const QString& fileName)
{
  Q_D(voDelimitedTextImportDialog);
  this->setWindowTitle(QString("Import Data - ") + fileName);
  d->DelimitedTextPreviewModel.setFileName(fileName);

  // Apply default normalization to data
  d->NormalizationWidget->setSelectedNormalizationMethod(d->NormalizationWidget->selectedNormalizationMethod());
}

// --------------------------------------------------------------------------
voDelimitedTextImportSettings voDelimitedTextImportDialog::importSettings()const
{
  Q_D(const voDelimitedTextImportDialog);
  voDelimitedTextImportSettings settings;

  // vtkDelimitedTextReader settings
  settings.insert(voDelimitedTextImportSettings::FieldDelimiterCharacters,
                  d->DelimitedTextPreviewModel.fieldDelimiterCharacters());
  settings.insert(voDelimitedTextImportSettings::MergeConsecutiveDelimiters,
                  d->DelimitedTextPreviewModel.mergeConsecutiveDelimiters());
  settings.insert(voDelimitedTextImportSettings::StringDelimiter,
                  d->DelimitedTextPreviewModel.stringDelimiter());
  settings.insert(voDelimitedTextImportSettings::UseStringDelimiter,
                  d->DelimitedTextPreviewModel.useStringDelimiter());
  // vtkExtendedTable settings
  settings.insert(voDelimitedTextImportSettings::Transpose,
                  d->DelimitedTextPreviewModel.transpose());
  settings.insert(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes,
                  d->DelimitedTextPreviewModel.numberOfColumnMetaDataTypes());
  settings.insert(voDelimitedTextImportSettings::ColumnMetaDataTypeOfInterest,
                  d->DelimitedTextPreviewModel.columnMetaDataTypeOfInterest());
  settings.insert(voDelimitedTextImportSettings::NumberOfRowMetaDataTypes,
                  d->DelimitedTextPreviewModel.numberOfRowMetaDataTypes());
  settings.insert(voDelimitedTextImportSettings::RowMetaDataTypeOfInterest,
                  d->DelimitedTextPreviewModel.rowMetaDataTypeOfInterest());
  // Normalization settings
  settings.insert(voDelimitedTextImportSettings::NormalizationMethod,
                  d->NormalizationWidget->selectedNormalizationMethod());

  return settings;
}

#include <QDebug>
// --------------------------------------------------------------------------
void voDelimitedTextImportDialog::setNormalizationMethod(const QString& normalizationMethodName)
{
  Q_D(voDelimitedTextImportDialog);
  d->NormalizationMethodLabel->setText(d->preProcessingAndFilteringGroupBoxText(normalizationMethodName));

  voDelimitedTextPreviewModel * model = &d->DelimitedTextPreviewModel;

  // Apply normalization to document preview
  model->resetDataTable();
  voApplication::application()->normalizerRegistry()->apply(
        normalizationMethodName, model->dataTable(), QHash<int, QVariant>());
}
