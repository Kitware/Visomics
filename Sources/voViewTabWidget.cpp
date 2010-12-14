
// Visomics includes
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
