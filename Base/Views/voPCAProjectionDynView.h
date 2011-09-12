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


#ifndef __voPCAProjectionDynView_h
#define __voPCAProjectionDynView_h

// Visomics includes
#include "voDynView.h"

class voPCAProjectionDynViewPrivate;

class voPCAProjectionDynView : public voDynView
{
  Q_OBJECT
public:
  typedef voDynView Superclass;
  voPCAProjectionDynView(QWidget * newParent = 0);
  virtual ~voPCAProjectionDynView();

protected:
  virtual QString stringify(const voDataObject &dataObject);

protected:
  QScopedPointer<voPCAProjectionDynViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voPCAProjectionDynView);
  Q_DISABLE_COPY(voPCAProjectionDynView);
};

#endif
