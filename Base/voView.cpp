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
#include <QLabel>
#include <QVBoxLayout>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkDataObject.h>

//----------------------------------------------------------------------------
class voViewPrivate
{
public:
};

// --------------------------------------------------------------------------
// voViewPrivate methods

// --------------------------------------------------------------------------
// voView methods

// --------------------------------------------------------------------------
voView::voView(QWidget* newParent) : Superclass(newParent), d_ptr(new voViewPrivate)
{
}

// --------------------------------------------------------------------------
voView::~voView()
{
}

// --------------------------------------------------------------------------
void voView::initialize()
{
  QVBoxLayout * verticalLayout = new QVBoxLayout(this);
  verticalLayout->setSpacing(0);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  this->setupUi(verticalLayout);
  QLabel * hintsLabel = new QLabel(this->hints());
  hintsLabel->setWordWrap(true);
  hintsLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  verticalLayout->addWidget(hintsLabel);
}

// --------------------------------------------------------------------------
QString voView::hints()const
{
  return QString();
}

