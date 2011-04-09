
// Visomics includes
#include "voDelimitedTextImportWidget.h"
#include "ui_voDelimitedTextImportWidget.h"
#include "voDelimitedTextPreviewModel.h"

class voDelimitedTextImportWidgetPrivate : public Ui_voDelimitedTextImportWidget
{
public:
  voDelimitedTextImportWidgetPrivate();

  voDelimitedTextPreviewModel DelimitedTextPreviewModel;
};

// --------------------------------------------------------------------------
// voDelimitedTextImportWidgetPrivatePrivate methods

// --------------------------------------------------------------------------
voDelimitedTextImportWidgetPrivate::voDelimitedTextImportWidgetPrivate()
{
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
  d->DelimitedTextPreviewModel.updatePreview();
}

