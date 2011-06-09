
// Qt includes
#include <QDebug>

// Visomics includes
#include "voViewFactory.h"
#include "voQObjectFactory.h"
#include "voView.h"

#include "voCorrelationGraphView.h"
#include "voHeatMapView.h"
#include "voExtendedTableView.h"
#include "voHorizontalBarView.h"
#include "voPCAProjectionView.h"
#include "voPCABarView.h"
#include "voTableView.h"
#include "voTreeGraphView.h"
#include "voVolcanoView.h"

//----------------------------------------------------------------------------
class voViewFactoryPrivate
{
public:

  voQObjectFactory<voView>     ViewFactory;
};

//----------------------------------------------------------------------------
// voViewFactoryPrivate methods

//----------------------------------------------------------------------------
// voViewFactory methods

//----------------------------------------------------------------------------
voViewFactory::voViewFactory():d_ptr(new voViewFactoryPrivate)
{
  this->registerView<voCorrelationGraphView>();
  this->registerView<voHeatMapView>();
  this->registerView<voExtendedTableView>();
  this->registerView<voHorizontalBarView>();
  this->registerView<voPCABarView>();
  this->registerView<voPCAProjectionView>();
  this->registerView<voTableView>();
  this->registerView<voTreeGraphView>();
  this->registerView<voVolcanoView>();
}

//-----------------------------------------------------------------------------
voViewFactory::~voViewFactory()
{
}

//-----------------------------------------------------------------------------
voView* voViewFactory::createView(const QString& className)
{
  Q_D(voViewFactory);
  voView * newView = d->ViewFactory.Create(className);
  if (newView)
    {
    newView->initialize();
    }
  return newView;
}

//-----------------------------------------------------------------------------
QStringList voViewFactory::registeredViewNames() const
{
  Q_D(const voViewFactory);
  return d->ViewFactory.registeredObjectKeys();
}

//-----------------------------------------------------------------------------
template<typename ClassType>
void voViewFactory::registerView(const QString& viewName)
{
  Q_D(voViewFactory);

  QString viewClassName(ClassType::staticMetaObject.className());

  QString tmpViewName = viewName;
  if (viewName.isEmpty())
    {
    tmpViewName = viewClassName;
    }
    
  if (this->registeredViewNames().contains(tmpViewName))
    {
    return;
    }

  d->ViewFactory.registerObject<ClassType>(tmpViewName);
}
