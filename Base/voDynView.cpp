
// Qt includes
#include <QDebug>
#include <QExplicitlySharedDataPointer>
#include <QLayout>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>

// Visomics includes
#include "voApplication.h"
#include "voConfigure.h" // For Visomics_SOURCE_DIR
#include "voDataObject.h"
#include "voDynView.h"
#include "voUtils.h"

// VTK includes
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voDynViewPrivate
{
public:
  voDynViewPrivate();

  QWebFrame* mainFrame();

  QString                       ViewName;
  QWebView*                     Widget;
  QExplicitlySharedDataPointer<voDataObject>  DataObject;
};

// --------------------------------------------------------------------------
// voDynViewPrivate methods

// --------------------------------------------------------------------------
voDynViewPrivate::voDynViewPrivate()
{
  this->Widget = 0;
}

// --------------------------------------------------------------------------
QWebFrame* voDynViewPrivate::mainFrame()
{
  return this->Widget->page()->mainFrame();
}

// --------------------------------------------------------------------------
// voDynView methods

// --------------------------------------------------------------------------
voDynView::voDynView(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voDynViewPrivate)
{
}

// --------------------------------------------------------------------------
voDynView::~voDynView()
{
}

// --------------------------------------------------------------------------
void voDynView::setupUi(QLayout * layout)
{
  Q_D(voDynView);
  d->Widget = new QWebView;
  qDebug() << "htmlFilePath" << this->htmlFilePath();
  this->d_ptr->Widget->setUrl(this->htmlFilePath());
  this->d_ptr->Widget->show();
  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
QString voDynView::htmlFilePath()const
{
  Q_D(const voDynView);
  voApplication * app = voApplication::application();
  QString dynamicViewSubDirectory;
  if(app->isInstalled())
    {
    dynamicViewSubDirectory = app->homeDirectory() + QLatin1String("/share/dynamicviews/");
    }
  else
    {
    dynamicViewSubDirectory = QLatin1String(Visomics_SOURCE_DIR)
        + QLatin1String("/Base/Views/Resources/dynamicviews/");
    }
  return QLatin1String("file://") + dynamicViewSubDirectory + d->ViewName
      + QLatin1String("/") + d->ViewName + QLatin1String(".html");
}

// --------------------------------------------------------------------------
void voDynView::initialize()
{
  Q_D(voDynView);

  QRegExp rx("^vo(.+)DynView$");
  bool match = rx.exactMatch(this->metaObject()->className());
  Q_ASSERT(match);
  d->ViewName = rx.cap(1);

  this->Superclass::initialize();
}

// --------------------------------------------------------------------------
QString voDynView::stringify(const voDataObject& dataObject)
{
  Q_UNUSED(dataObject)
  return QString();
}

// --------------------------------------------------------------------------
void voDynView::loadDataObject()
{
  Q_D(voDynView);
  if (!d->DataObject)
    {
    qCritical() << "voDynView - Failed to loadDataObject - DataObject is NULL";
    return;
    }
  d->mainFrame()->addToJavaScriptWindowObject(QLatin1String("dataobject"), d->DataObject.data());
}

// --------------------------------------------------------------------------
void voDynView::setDataObject(voDataObject *dataObject)
{
  Q_D(voDynView);

  if (!dataObject)
    {
    qCritical() << "voDynView - Failed to setDataObject - dataObject is NULL";
    return;
    }
  dataObject->setProperty("json", this->stringify(*dataObject));

  d->DataObject = QExplicitlySharedDataPointer<voDataObject>(dataObject);
  this->loadDataObject();
  connect(d->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(loadDataObject()));
}

// --------------------------------------------------------------------------
QString voDynView::viewName()const
{
  Q_D(const voDynView);
  return d->ViewName;
}
