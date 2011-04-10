
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
          d->NumberHeaderRowsSpinBox, SLOT(setValue(int)));

  connect(&d->DelimitedTextPreviewModel, SIGNAL(numberOfRowMetaDataTypesChanged(int)),
          d->NumberHeaderColumnsSpinBox, SLOT(setValue(int)));

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

