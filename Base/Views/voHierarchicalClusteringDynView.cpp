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
#include "voHierarchicalClusteringDynView.h"
#include "voUtils.h"

// VTK includes
#include <vtkTree.h>
#include <vtkGraph.h>
#include <vtkTreeLevelsFilter.h>
#include <vtkDataSetAttributes.h>
#include <vtkAbstractArray.h>

// --------------------------------------------------------------------------
class voHierarchicalClusteringDynViewPrivate
{
public:
  voHierarchicalClusteringDynViewPrivate();
};

// --------------------------------------------------------------------------
// voHierarchicalClusteringDynViewPrivate methods

// --------------------------------------------------------------------------
voHierarchicalClusteringDynViewPrivate::voHierarchicalClusteringDynViewPrivate()
{
}

// --------------------------------------------------------------------------
// voHierarchicalClusteringDynView methods

// --------------------------------------------------------------------------
voHierarchicalClusteringDynView::voHierarchicalClusteringDynView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voHierarchicalClusteringDynViewPrivate)
{
}

// --------------------------------------------------------------------------
voHierarchicalClusteringDynView::~voHierarchicalClusteringDynView()
{
}

// --------------------------------------------------------------------------
QString voHierarchicalClusteringDynView::stringify(const voDataObject& dataObject)
{
  vtkTree * tree = vtkTree::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "voHierarchicalClusteringDynView - Failed to setDataObject - vtkTree data is expected !";
    return QString();
    }

  return voUtils::stringify(this->viewName(), tree);
}
