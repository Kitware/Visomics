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


#ifndef __voTreeHeatmapView_h
#define __voTreeHeatmapView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voTreeHeatmapViewPrivate;

class voTreeHeatmapView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voTreeHeatmapView(QWidget * newParent = 0);
  virtual ~voTreeHeatmapView();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectListInternal(const QList<voDataObject*> dataObjects);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

protected:
  QScopedPointer<voTreeHeatmapViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTreeHeatmapView);
  Q_DISABLE_COPY(voTreeHeatmapView);
};

#endif
