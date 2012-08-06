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

// Visomics includes
#include "voViewFactory.h"
#include "voQObjectFactory.h"
#include "voView.h"

#include "voCorrelationGraphView.h"
#include "voHeatMapView.h"
#include "voHierarchicalClusteringDynView.h"
#include "voExtendedTableView.h"
#include "voHorizontalBarView.h"
#include "voKEGGImageView.h"
#include "voKEGGPathwayView.h"
#include "voKEGGTableView.h"
#include "voKMeansClusteringDynView.h"
#include "voPCAProjectionDynView.h"
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
  this->registerView<voExtendedTableView>();
  this->registerView<voHeatMapView>();
  this->registerView<voHierarchicalClusteringDynView>();
  this->registerView<voHorizontalBarView>();
  this->registerView<voKEGGImageView>();
  this->registerView<voKEGGPathwayView>();
  this->registerView<voKEGGTableView>();
  this->registerView<voKMeansClusteringDynView>();
  this->registerView<voPCABarView>();
  this->registerView<voPCAProjectionDynView>();
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
