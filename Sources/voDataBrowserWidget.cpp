

// Visomics includes
#include "voDataBrowserWidget.h"

// --------------------------------------------------------------------------
voDataBrowserWidget::voDataBrowserWidget(QWidget* newParent) : Superclass(newParent)
{
  QStringList fileTreeWidgetLabel;
  fileTreeWidgetLabel << tr ("Data browser");
  this->setHeaderLabels(fileTreeWidgetLabel);
}

// --------------------------------------------------------------------------
voDataBrowserWidget::~voDataBrowserWidget()
{
}
