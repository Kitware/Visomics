
// Qt includes
#include <QGroupBox>
#include <QLabel>

// Visomics includes
#include "ui_voDelimitedTextImportDialog.h"
#include "voDelimitedTextImportDialog.h"
#include "voDelimitedTextImportWidget.h"
#include "voDelimitedTextPreviewModel.h"

class voDelimitedTextImportDialogPrivate : public Ui_voDelimitedTextImportDialog
{
public:
  typedef Ui_voDelimitedTextImportDialog Superclass;

  voDelimitedTextImportDialogPrivate();

  void setupUi(QDialog *widget);
};

// --------------------------------------------------------------------------
// voDelimitedTextImportDialogPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextImportDialogPrivate::voDelimitedTextImportDialogPrivate()
{
}

// --------------------------------------------------------------------------
void voDelimitedTextImportDialogPrivate::setupUi(QDialog *widget)
{
  this->Superclass::setupUi(widget);

  // Add "PreProcessing&Filtering" groupbox to DelimitedTextImportWidget
  QGroupBox * preProcessingAndFilteringGroupBox = new QGroupBox("Pre-processing && Filtering");
  QVBoxLayout * layout = new QVBoxLayout(preProcessingAndFilteringGroupBox);
  layout->addWidget(new QLabel("<ul><li>Default normalization technique is XXXX. "
                               "Click on <i>Normalization</i> tab for more options.</li></ul>"));
  this->DelimitedTextImportWidget->insertWidget(
        preProcessingAndFilteringGroupBox, voDelimitedTextImportWidget::RowsAndColumnsGroupBox);

  preProcessingAndFilteringGroupBox->setDisabled(true);
  this->ImportStepTabWidget->setTabEnabled(1, false);
  this->ImportStepTabWidget->setTabEnabled(2, false);
}

// --------------------------------------------------------------------------
// voDelimitedTextImportDialog methods

// --------------------------------------------------------------------------
voDelimitedTextImportDialog::voDelimitedTextImportDialog(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voDelimitedTextImportDialogPrivate())
{
  Q_D(voDelimitedTextImportDialog);
  d->setupUi(this);
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
  d->DelimitedTextImportWidget->setFileName(fileName);
}

// --------------------------------------------------------------------------
voDelimitedTextImportSettings voDelimitedTextImportDialog::importSettings()const
{
  Q_D(const voDelimitedTextImportDialog);
  voDelimitedTextImportSettings settings;

  voDelimitedTextPreviewModel * model = d->DelimitedTextImportWidget->delimitedTextPreviewModel();

  // vtkDelimitedTextReader settings
  settings.insert(voDelimitedTextImportSettings::FieldDelimiterCharacters,
                  model->fieldDelimiterCharacters());
  settings.insert(voDelimitedTextImportSettings::MergeConsecutiveDelimiters,
                  model->mergeConsecutiveDelimiters());
  settings.insert(voDelimitedTextImportSettings::StringDelimiter,
                  model->stringDelimiter());
  settings.insert(voDelimitedTextImportSettings::UseStringDelimiter,
                  model->useStringDelimiter());
  settings.insert(voDelimitedTextImportSettings::HaveHeaders,
                  model->haveHeaders());
  // vtkExtendedTable settings
  settings.insert(voDelimitedTextImportSettings::Transpose,
                  model->transpose());
  settings.insert(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes,
                  model->numberOfColumnMetaDataTypes());
  settings.insert(voDelimitedTextImportSettings::ColumnMetaDataTypeOfInterest,
                  model->columnMetaDataTypeOfInterest());
  settings.insert(voDelimitedTextImportSettings::NumberOfRowMetaDataTypes,
                  model->numberOfRowMetaDataTypes());
  settings.insert(voDelimitedTextImportSettings::RowMetaDataTypeOfInterest,
                  model->rowMetaDataTypeOfInterest());

  return settings;
}
