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

  newView->centerData();
}
