
// Visomics includes
#include "voDelimitedTextImportWidget.h"
#include "ui_voDelimitedTextImportWidget.h"
#include "voDelimitedTextPreviewModel.h"

class voDelimitedTextImportWidgetPrivate : public Ui_voDelimitedTextImportWidget
{
public:
  voDelimitedTextImportWidgetPrivate();

  void updateWidgetFromModel();

  voDelimitedTextPreviewModel DelimitedTextPreviewModel;
};

// --------------------------------------------------------------------------
// voDelimitedTextImportWidgetPrivatePrivate methods

// --------------------------------------------------------------------------
voDelimitedTextImportWidgetPrivate::voDelimitedTextImportWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidgetPrivate::updateWidgetFromModel()
{
  this->TransposeCheckBox->setChecked(this->DelimitedTextPreviewModel.transpose());
  this->NumberHeaderColumnsSpinBox->setValue(this->DelimitedTextPreviewModel.numberOfRowMetaDataTypes());
  this->NumberHeaderRowsSpinBox->setValue(this->DelimitedTextPreviewModel.numberOfColumnMetaDataTypes());
}

// --------------------------------------------------------------------------
// voDelimitedTextImportWidget methods

// --------------------------------------------------------------------------
voDelimitedTextImportWidget::voDelimitedTextImportWidget(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voDelimitedTextImportWidgetPrivate())
{
  Q_D(voDelimitedTextImportWidget);
  d->setupUi(this);

  d->DocumentPreviewWidget->setModel(&d->DelimitedTextPreviewModel);

  d->updateWidgetFromModel();

  // Widget -> Model connections
  connect(d->TransposeCheckBox, SIGNAL(toggled(bool)),
          &d->DelimitedTextPreviewModel, SLOT(setTranspose(bool)));

  connect(d->NumberHeaderColumnsSpinBox, SIGNAL(valueChanged(int)),
          &d->DelimitedTextPreviewModel, SLOT(setNumberOfRowMetaDataTypes(int)));

  connect(d->NumberHeaderRowsSpinBox, SIGNAL(valueChanged(int)),
          &d->DelimitedTextPreviewModel, SLOT(setNumberOfColumnMetaDataTypes(int)));

  // Model -> Widget connections
  connect(&d->DelimitedTextPreviewModel, SIGNAL(numberOfColumnMetaDataTypesChanged(int)),
          this, SLOT(onNumberOfColumnMetaDataTypesChanged(int)));

  connect(&d->DelimitedTextPreviewModel, SIGNAL(numberOfRowMetaDataTypesChanged(int)),
          this, SLOT(onNumberOfRowMetaDataTypesChanged(int)));
}

// --------------------------------------------------------------------------
voDelimitedTextImportWidget::~voDelimitedTextImportWidget()
{
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::setFileName(const QString& fileName)
{
  Q_D(voDelimitedTextImportWidget);
  d->DelimitedTextPreviewModel.setFileName(fileName);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onNumberOfColumnMetaDataTypesChanged(int value)
{
  Q_D(voDelimitedTextImportWidget);
  d->NumberHeaderRowsSpinBox->setValue(value);
  d->DocumentPreviewWidget->horizontalHeader()->setVisible(value > 0);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onNumberOfRowMetaDataTypesChanged(int value)
{
  Q_D(voDelimitedTextImportWidget);
  d->NumberHeaderColumnsSpinBox->setValue(value);
  d->DocumentPreviewWidget->verticalHeader()->setVisible(value > 0);
}

