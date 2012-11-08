/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QLayout>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#include <QDesktopServices>

// Visomics includes
#include "voApplication.h"
#include "voConfigure.h" // For Visomics_SOURCE_DIR
#include "voDataObject.h"
#include "voDynView.h"
#include "voUtils.h"
#include "voJavascriptBridge.h"
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
  this->jsBridge = new voJavascriptBridge();

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
  return dynamicViewSubDirectory + d->ViewName
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
void voDynView::loadJavaScriptBridge()
{
  Q_D(voDynView);
  d->mainFrame()->addToJavaScriptWindowObject("jsBridge", this->jsBridge);
}
// --------------------------------------------------------------------------
void voDynView::loadDataObject()
{
  Q_D(voDynView);
  d->mainFrame()->addToJavaScriptWindowObject(QLatin1String("dataobject"), this->dataObject());
}

// --------------------------------------------------------------------------
void voDynView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voDynView);
  const_cast<voDataObject*>(&dataObject)->setProperty("json", this->stringify(dataObject));
  connect(d->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(loadDataObject()));


  d->Widget->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(d->Widget->page(), SIGNAL(linkClicked(const QUrl &)),
                         SLOT(newWindowOnLinkClicked(const QUrl &)));

  connect(d->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(loadJavaScriptBridge()));


  d->Widget->reload();
}




// --------------------------------------------------------------------------
void voDynView::newWindowOnLinkClicked(const QUrl & url)
{
  QDesktopServices::openUrl(url);
}

// --------------------------------------------------------------------------
QString voDynView::viewName()const
{
  Q_D(const voDynView);
  return d->ViewName;
}
