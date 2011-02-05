
// Visomics includes
#include "voDelimitedTextImportWidget.h"
#include "ui_voDelimitedTextImportWidget.h"

class voDelimitedTextImportWidgetPrivate : public Ui_voDelimitedTextImportWidget
{
public:
  voDelimitedTextImportWidgetPrivate();
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
  Superclass(newParent)
{
  Q_D(voDelimitedTextImportWidget);
  d->setupUi(this);
}

// --------------------------------------------------------------------------
voDelimitedTextImportWidget::~voDelimitedTextImportWidget()
{
}

