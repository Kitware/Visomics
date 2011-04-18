
// Visomics includes
#include "voDelimitedTextImportWidget.h"
#include "ui_voDelimitedTextImportWidget.h"
#include "voDelimitedTextPreviewModel.h"

class voDelimitedTextImportWidgetPrivate : public Ui_voDelimitedTextImportWidget
{
public:
  voDelimitedTextImportWidgetPrivate();

  void updateWidgetFromModel();

  QButtonGroup DelimiterButtonGroup;
  voDelimitedTextPreviewModel DelimitedTextPreviewModel;
};

// --------------------------------------------------------------------------
// voDelimitedTextImportWidgetPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextImportWidgetPrivate::voDelimitedTextImportWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidgetPrivate::updateWidgetFromModel()
{
  this->TransposeCheckBox->setChecked(this->DelimitedTextPreviewModel.transpose());

  this->NumberHeaderColumnsSpinBox->setValue(this->DelimitedTextPreviewModel.numberOfRowMetaDataTypes());

  this->HeaderColumnOfInterestSpinBox->setValue(this->DelimitedTextPreviewModel.rowMetaDataTypeOfInterest());
  this->HeaderColumnOfInterestSpinBox->setMaximum(this->DelimitedTextPreviewModel.numberOfRowMetaDataTypes()-1);

  this->NumberHeaderRowsSpinBox->setValue(this->DelimitedTextPreviewModel.numberOfColumnMetaDataTypes());

  this->HeaderRowOfInterestSpinBox->setValue(this->DelimitedTextPreviewModel.columnMetaDataTypeOfInterest());
  this->HeaderRowOfInterestSpinBox->setMaximum(this->DelimitedTextPreviewModel.numberOfColumnMetaDataTypes()-1);
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

  d->DelimiterButtonGroup.addButton(d->CommaRadioButton, ',');
  d->DelimiterButtonGroup.addButton(d->SemicolonRadioButton, ';');
  d->DelimiterButtonGroup.addButton(d->TabRadioButton, '\t');
  d->DelimiterButtonGroup.addButton(d->SpaceRadioButton, ' ');
  d->DelimiterButtonGroup.addButton(d->OtherRadioButton, 'x');

  // Delimiter connections
  connect(&d->DelimiterButtonGroup, SIGNAL(buttonClicked(int)),
          this, SLOT(onDelimiterChanged(int)));

  // StringBeginEndCharacter connection
  connect(d->StringDelimiterCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(onStringDelimiterEnabled(bool)));

  // Widget -> Model connections
  connect(d->TransposeCheckBox, SIGNAL(toggled(bool)),
          &d->DelimitedTextPreviewModel, SLOT(setTranspose(bool)));

  connect(d->NumberHeaderColumnsSpinBox, SIGNAL(valueChanged(int)),
          &d->DelimitedTextPreviewModel, SLOT(setNumberOfRowMetaDataTypes(int)));

  connect(d->HeaderColumnOfInterestSpinBox, SIGNAL(valueChanged(int)),
          &d->DelimitedTextPreviewModel, SLOT(setRowMetaDataTypeOfInterest(int)));

  connect(d->NumberHeaderRowsSpinBox, SIGNAL(valueChanged(int)),
          &d->DelimitedTextPreviewModel, SLOT(setNumberOfColumnMetaDataTypes(int)));

  connect(d->HeaderRowOfInterestSpinBox, SIGNAL(valueChanged(int)),
          &d->DelimitedTextPreviewModel, SLOT(setColumnMetaDataTypeOfInterest(int)));

  // Model -> Widget connections
  connect(&d->DelimitedTextPreviewModel, SIGNAL(numberOfColumnMetaDataTypesChanged(int)),
          this, SLOT(onNumberOfColumnMetaDataTypesChanged(int)));
  connect(&d->DelimitedTextPreviewModel, SIGNAL(columnMetaDataTypeOfInterestChanged(int)),
          this, SLOT(onColumnMetaDataTypeOfInterestChanged(int)));

  connect(&d->DelimitedTextPreviewModel, SIGNAL(numberOfRowMetaDataTypesChanged(int)),
          this, SLOT(onNumberOfRowMetaDataTypesChanged(int)));
  connect(&d->DelimitedTextPreviewModel, SIGNAL(rowMetaDataTypeOfInterestChanged(int)),
          this, SLOT(onRowMetaDataTypeOfInterestChanged(int)));
}

// --------------------------------------------------------------------------
voDelimitedTextImportWidget::~voDelimitedTextImportWidget()
{
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::insertWidget(QWidget * widget, InsertWidgetLocation location)
{
  Q_D(voDelimitedTextImportWidget);
  if (!widget)
    {
    return;
    }
  int index = -1;
  if (location == Self::DelimiterGroupBox)
    {
    index = d->MainVerticalLayout->indexOf(d->DelimiterGroupBox);
    }
  else if (location == Self::RowsAndColumnsGroupBox)
    {
    index = d->MainVerticalLayout->indexOf(d->RowsColumnsGroupBox);
    }
  else if (location == Self::DocumentPreviewGroupBox)
    {
    index = d->MainVerticalLayout->indexOf(d->DocumentPreviewGroupBox);
    }
  Q_ASSERT(index != -1);
  d->MainVerticalLayout->insertWidget(index, widget);
}

// --------------------------------------------------------------------------
voDelimitedTextPreviewModel* voDelimitedTextImportWidget::delimitedTextPreviewModel()
{
  Q_D(voDelimitedTextImportWidget);
  return &d->DelimitedTextPreviewModel;
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

  // Modify HeaderRowOfInterestSpinBox to conform
  d->HeaderRowOfInterestSpinBox->setEnabled(value != 0);
  if(value > 0)
    {
    d->HeaderRowOfInterestSpinBox->setMaximum(value-1);
    }
  if(value < d->HeaderRowOfInterestSpinBox->value())
    {
    d->HeaderRowOfInterestSpinBox->setValue(value);
    }

  d->DocumentPreviewWidget->horizontalHeader()->setVisible(value > 0);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onColumnMetaDataTypeOfInterestChanged(int value)
{
  Q_D(voDelimitedTextImportWidget);
  d->HeaderRowOfInterestSpinBox->setValue(value);
  //set header here?
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onNumberOfRowMetaDataTypesChanged(int value)
{
  Q_D(voDelimitedTextImportWidget);
  d->NumberHeaderColumnsSpinBox->setValue(value);

  // Modify HeaderColumnOfInterestSpinBox to conform
  d->HeaderColumnOfInterestSpinBox->setEnabled(value != 0);
  if(value > 0)
    {
    d->HeaderColumnOfInterestSpinBox->setMaximum(value-1);
    }
  if(value < d->HeaderColumnOfInterestSpinBox->value())
    {
    d->HeaderColumnOfInterestSpinBox->setValue(value);
    }

  d->DocumentPreviewWidget->verticalHeader()->setVisible(value > 0);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onRowMetaDataTypeOfInterestChanged(int value)
{
  Q_D(voDelimitedTextImportWidget);
  d->HeaderColumnOfInterestSpinBox->setValue(value);
  //set header here?
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onDelimiterChanged(int delimiter)
{
  Q_D(voDelimitedTextImportWidget);
  if (delimiter == 'x')
    { // Special case triggered by OtherRadioButton
    QString text = d->OtherLineEdit->text();

    connect(d->OtherLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(onOtherDelimiterLineEditChanged(const QString&)));

    if (text.isEmpty())
      {
      return;
      }
    delimiter = text.at(0).toLatin1();
    }
  else
    {
    disconnect(d->OtherLineEdit, SIGNAL(textChanged(const QString&)),
               this, SLOT(onOtherDelimiterLineEditChanged(const QString&)));
    }
  d->DelimitedTextPreviewModel.setFieldDelimiter(delimiter);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onOtherDelimiterLineEditChanged(const QString& text)
{
  Q_D(voDelimitedTextImportWidget);
  if (text.isEmpty())
    {
    return;
    }
  char delimiter = d->OtherLineEdit->text().at(0).toLatin1();
  d->DelimitedTextPreviewModel.setFieldDelimiter(delimiter);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onStringDelimiterEnabled(bool value)
{
  Q_D(voDelimitedTextImportWidget);
  char character = 0;
  if (value)
    {
    QString text = d->StringDelimiterLineEdit->text();

    connect(d->StringDelimiterLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(onStringDelimiterLineEditChanged(const QString&)));

    if (text.isEmpty())
      {
      return;
      }
    character = text.at(0).toLatin1();
    }
  else
    {
    disconnect(d->StringDelimiterLineEdit, SIGNAL(textChanged(const QString&)),
               this, SLOT(onStringDelimiterLineEditChanged(const QString&)));
    }
  d->DelimitedTextPreviewModel.setStringDelimiter(character);
}

// --------------------------------------------------------------------------
void voDelimitedTextImportWidget::onStringDelimiterLineEditChanged(const QString& text)
{
  Q_D(voDelimitedTextImportWidget);
  if (text.isEmpty())
    {
    return;
    }
  char character = d->StringDelimiterLineEdit->text().at(0).toLatin1();
  d->DelimitedTextPreviewModel.setStringDelimiter(character);
}
