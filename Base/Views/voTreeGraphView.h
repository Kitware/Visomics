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


#ifndef __voTreeGraphView_h
#define __voTreeGraphView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voTreeGraphViewPrivate;

class voTreeGraphView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voTreeGraphView(QWidget * newParent = 0);
  virtual ~voTreeGraphView();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

protected:
  QScopedPointer<voTreeGraphViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTreeGraphView);
  Q_DISABLE_COPY(voTreeGraphView);
};

#endif
