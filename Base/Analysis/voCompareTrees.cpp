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
#include "voCompareTrees.h"
#include "voInputFileDataObject.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkNew.h>
#include <vtkTree.h>
#include <vtkTreeDifferenceFilter.h>

// --------------------------------------------------------------------------
// voCompareTrees methods

// --------------------------------------------------------------------------
voCompareTrees::voCompareTrees(): Superclass()
{
}

// --------------------------------------------------------------------------
voCompareTrees::~voCompareTrees()
{
}

// --------------------------------------------------------------------------
void voCompareTrees::setOutputInformation()
{
  this->addOutputType("outputTree", "vtkTree",
                      "voTreeHeatmapView", "Tree Difference",
                      "", "");
}

// --------------------------------------------------------------------------
void voCompareTrees::setParameterInformation()
{
  this->addTreeParameter("comparison tree");
}

// --------------------------------------------------------------------------
QString voCompareTrees::parameterDescription()const
{

  return QString("<dl>"
                 "<dt><b>Comparison Tree</b>:</dt>"
                 "<dd>This is the tree that will be subtracted from your input tree.</dd>"
                 "</dl>");
}

// --------------------------------------------------------------------------
bool voCompareTrees::execute()
{
  // Input tree
  vtkTree* inputTree =  vtkTree::SafeDownCast(this->input(0)->dataAsVTKDataObject());
  if (!inputTree)
    {
    qCritical() << "Input tree is Null";
    return false;
    }

  // Comparison tree
  vtkTree *comparisonTree = this->treeParameter("comparison tree");
  if (!comparisonTree)
    {
    qCritical() << "Comparison tree is Null";
    return false;
    }

  vtkNew<vtkTreeDifferenceFilter> filter;
  filter->SetInputDataObject(0, inputTree);
  filter->SetInputDataObject(1, comparisonTree);
  filter->SetIdArrayName("node name");
  filter->SetComparisonArrayIsVertexData(true);
  filter->SetComparisonArrayName("node weight");
  filter->SetOutputArrayName("differences");
  filter->Update();

  vtkNew<vtkTree> outputTree;
  outputTree->ShallowCopy(filter->GetOutput());
  if (outputTree.GetPointer() == NULL)
    {
    qCritical() << QObject::tr("Error generated output tree.");
    return false;
    }

  this->setOutput("outputTree",
    new voInputFileDataObject("outputTree", outputTree.GetPointer()));

  return true;
}
