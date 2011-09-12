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


#ifndef __voVolcanoView_h
#define __voVolcanoView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voVolcanoViewPrivate;

class voVolcanoView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voVolcanoView(QWidget * newParent = 0);
  virtual ~voVolcanoView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voVolcanoViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voVolcanoView);
  Q_DISABLE_COPY(voVolcanoView);
};

#endif
