
// Visomics includes
#include "voApplication.h"
#include "voViewStackedWidget.h"
#include "voView.h"

// --------------------------------------------------------------------------
class voViewStackedWidgetPrivate
{
public:
  
};

// --------------------------------------------------------------------------
// voViewStackedWidgetPrivate methods

// --------------------------------------------------------------------------
// voViewStackedWidget methods

// --------------------------------------------------------------------------
voViewStackedWidget::voViewStackedWidget(QWidget* newParent):Superclass(newParent), 
  d_ptr(new voViewStackedWidgetPrivate)
{
}

// --------------------------------------------------------------------------
voViewStackedWidget::~voViewStackedWidget()
{
}

// --------------------------------------------------------------------------
void voViewStackedWidget::addView(const QString& /*objectUuid*/, voView * newView)
{
  if (!newView)
    {
    return;
    }
  int viewIndex = this->indexOf(newView);
  if (viewIndex == -1)
    {
//    voDataModelItem* item =
//        voApplication::application()->dataModel()->findItemWithUuid(objectUuid);

    viewIndex = this->addWidget(newView);
    }
  this->setCurrentIndex(viewIndex);
}
