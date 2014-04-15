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
#include <QList>

// Visomics includes
#include "voDataObject.h"
#include "voPCAProjectionDynView.h"
#include "voUtils.h"

// VTK includes
#include <vtkNew.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voPCAProjectionDynViewPrivate
{
public:
  voPCAProjectionDynViewPrivate();
};

// --------------------------------------------------------------------------
// voPCAProjectionDynViewPrivate methods

// --------------------------------------------------------------------------
voPCAProjectionDynViewPrivate::voPCAProjectionDynViewPrivate()
{
}

// --------------------------------------------------------------------------
// voPCAProjectionDynView methods

// --------------------------------------------------------------------------
voPCAProjectionDynView::voPCAProjectionDynView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voPCAProjectionDynViewPrivate)
{
}

// --------------------------------------------------------------------------
voPCAProjectionDynView::~voPCAProjectionDynView()
{
}

// --------------------------------------------------------------------------
QString voPCAProjectionDynView::stringify(const voDataObject& dataObject)
{
  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voPCAProjectionDynView - Failed to setDataObject - vtkTable data is expected !";
    return QString();
    }

  vtkNew<vtkTable> transpose;
  voUtils::transposeTable(table, transpose.GetPointer(), voUtils::Headers);
  return voUtils::stringify(this->viewName(), transpose.GetPointer(), QList<vtkIdType>() << 0);
}
