
// Qt includes
#include <QDebug>

// Visomics includes
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voDataBrowserWidget.h"

// --------------------------------------------------------------------------
voDataBrowserWidget::voDataBrowserWidget(QWidget* newParent) : Superclass(newParent)
{
  this->setHeaderHidden(true);
}

// --------------------------------------------------------------------------
voDataBrowserWidget::~voDataBrowserWidget()
{
}

// --------------------------------------------------------------------------
void voDataBrowserWidget::setActiveAnalysis(voAnalysis* analysis)
{
  if (!analysis)
    {
    return;
    }

  voDataModel * dataModel = qobject_cast<voDataModel*>(this->model());
  Q_ASSERT(dataModel);
  voDataModelItem * item = dataModel->itemForAnalysis(analysis);
  this->expand(dataModel->indexFromItem(item));
}
