
// Visomics includes
#include "voApplication.h"
#include "voDataModelItem.h"
#include "voDataModel.h"
#include "voViewManager.h"
#include "voViewTabWidget.h"
#include "voView.h"

// --------------------------------------------------------------------------
class voViewTabWidgetPrivate
{
public:
  
};

// --------------------------------------------------------------------------
// voViewTabWidgetPrivate methods

// --------------------------------------------------------------------------
// voViewTabWidget methods

// --------------------------------------------------------------------------
voViewTabWidget::voViewTabWidget(QWidget* newParent):Superclass(newParent), 
  d_ptr(new voViewTabWidgetPrivate)
{
  this->setTabsClosable(true);
  this->setUsesScrollButtons(true);
  this->setMovable(true);
  this->setElideMode(Qt::ElideMiddle);
  this->connect(this, SIGNAL(tabCloseRequested(int)),
                SLOT(onTabCloseRequested(int)));
}

// --------------------------------------------------------------------------
voViewTabWidget::~voViewTabWidget()
{
}

// --------------------------------------------------------------------------
void voViewTabWidget::addViewTab(const QString& objectUuid, voView * newView)
{
  if (!newView)
    {
    return;
    }
  int viewIndex = this->indexOf(newView);
  if (viewIndex == -1)
    {
    voDataModelItem* item =
        voApplication::application()->dataModel()->findItemWithUuid(objectUuid);

    viewIndex = this->addTab(newView, item->icon(), newView->objectName());
    }
  this->setCurrentIndex(viewIndex);
}

// --------------------------------------------------------------------------
void voViewTabWidget::onTabCloseRequested(int index)
{
  voView* view = qobject_cast<voView*>(this->widget(index));
  if (view)
    {
    voApplication::application()->viewManager()->deleteView(view);
    }
}
