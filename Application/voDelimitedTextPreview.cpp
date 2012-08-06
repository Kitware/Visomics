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
#include <QHeaderView>

// Visomics includes
#include "voDelimitedTextPreview.h"

class voDelimitedTextPreviewPrivate
{
  Q_DECLARE_PUBLIC(voDelimitedTextPreview);

private:
  voDelimitedTextPreview* const q_ptr;

public:
  voDelimitedTextPreviewPrivate(voDelimitedTextPreview& object);
};

// --------------------------------------------------------------------------
// voDelimitedTextPreviewPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewPrivate::voDelimitedTextPreviewPrivate(voDelimitedTextPreview& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
// voDelimitedTextPreview methods

// --------------------------------------------------------------------------
voDelimitedTextPreview::voDelimitedTextPreview(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voDelimitedTextPreviewPrivate(*this))
{
  this->horizontalHeader()->setVisible(false);
  this->verticalHeader()->setVisible(false);
}

// --------------------------------------------------------------------------
voDelimitedTextPreview::~voDelimitedTextPreview()
{
}
