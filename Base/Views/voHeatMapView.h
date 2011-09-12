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


#ifndef __voHeatMapView_h
#define __voHeatMapView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voHeatMapViewPrivate;

class voHeatMapView : public voView
{
  Q_OBJECT
public:
  typedef voView Superclass;
  voHeatMapView(QWidget * newParent = 0);
  virtual ~voHeatMapView();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

protected:
  QScopedPointer<voHeatMapViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voHeatMapView);
  Q_DISABLE_COPY(voHeatMapView);
};

#endif
