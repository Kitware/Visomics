
// Visomics includes
#include "voApplication.h"
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

  this->connect(this, SIGNAL(tabCloseRequested(int)),
                SLOT(onTabCloseRequested(int)));
}

// --------------------------------------------------------------------------
voViewTabWidget::~voViewTabWidget()
{
}

// --------------------------------------------------------------------------
void voViewTabWidget::createView(const QString& objectUuid, voView * newView)
{
  if (!newView)
    {
    return;
    }
  int viewIndex = this->indexOf(newView);
  if (viewIndex == -1)
    {
    viewIndex = this->addTab(newView, newView->objectName());
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
