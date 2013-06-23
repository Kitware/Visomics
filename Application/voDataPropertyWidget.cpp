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
#include <QtTreePropertyBrowser>
#include <QtVariantEditorFactory>
#include <QtVariantPropertyManager>
#include <QPointer>
#include <QVariant>

// Visomics includes
#include "voDataObject.h"
#include "voDataModelItem.h"
#include "voDataPropertyWidget.h"

class voDataPropertyWidgetPrivate
{
  Q_DECLARE_PUBLIC(voDataPropertyWidget);

protected:
  voDataPropertyWidget* const q_ptr;

public:
  voDataPropertyWidgetPrivate(voDataPropertyWidget& object);

  QtVariantPropertyManager * PropertyManager;
  QtVariantEditorFactory* PropertyEditorFactory;
  QPointer<voDataObject> DataObject;

  void init();
};

// --------------------------------------------------------------------------
//voDataPropertyWidgetPrivate methods

// --------------------------------------------------------------------------
voDataPropertyWidgetPrivate::voDataPropertyWidgetPrivate(voDataPropertyWidget& object)
  :q_ptr(&object)
{

}

// --------------------------------------------------------------------------
void voDataPropertyWidgetPrivate::init()
{
 Q_Q(voDataPropertyWidget);

 q->setPropertiesWithoutValueMarked(true);
 q->setRootIsDecorated(true);
 q->setHeaderVisible(true);
}

// --------------------------------------------------------------------------
//voDataPropertyWidget methods

// --------------------------------------------------------------------------
voDataPropertyWidget::voDataPropertyWidget(QWidget* newParent) :
 Superclass(newParent), d_ptr(new voDataPropertyWidgetPrivate(*this))
{
  Q_D(voDataPropertyWidget);
  d->init();
}

// --------------------------------------------------------------------------
voDataPropertyWidget::~voDataPropertyWidget()
{
}

// --------------------------------------------------------------------------
void voDataPropertyWidget::setPropertyDataObject(voDataObject* dataObject)
{
 Q_D(voDataPropertyWidget);

 if ( !dataObject)
  {
  return;
  }

 d->DataObject = QPointer<voDataObject> (dataObject);
}

// --------------------------------------------------------------------------
void voDataPropertyWidget::setPropertyDataModelItem(voDataModelItem* dataModelItem)
{
 if ( !dataModelItem)
  {
  return;
  }

 this->clear();

 QtVariantPropertyManager * propertyManager =  dataModelItem->dataObject()->variantPropertyManager();

 /*
 //enable property editing
 QtVariantEditorFactory * propertyEditorFactory = new QtVariantEditorFactory();
 this->setFactoryForManager(propertyManager,propertyEditorFactory);
*/

 //add properties to the Data Property Panel
 foreach(QtProperty * prop, propertyManager->properties())
  {
  this->addProperty(prop);
  }

 this->show();
}
