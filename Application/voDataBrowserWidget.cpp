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
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voDataBrowserWidget.h"

// --------------------------------------------------------------------------
voDataBrowserWidget::voDataBrowserWidget(QWidget* newParent) : Superclass(newParent)
{
  this->setHeaderHidden(true);
}

// --------------------------------------------------------------------------
voDataBrowserWidget::~voDataBrowserWidget()
{
}

// --------------------------------------------------------------------------
void voDataBrowserWidget::setActiveAnalysis(voAnalysis* analysis)
{
  if (!analysis)
    {
    return;
    }

  voDataModel * dataModel = qobject_cast<voDataModel*>(this->model());
  Q_ASSERT(dataModel);
  voDataModelItem * item = dataModel->itemForAnalysis(analysis);
  this->expand(dataModel->indexFromItem(item));
}
