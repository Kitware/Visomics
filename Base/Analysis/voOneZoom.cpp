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

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voOneZoom.h"
#include "voDataObject.h"
#include "voInputFileDataObject.h"
#include "voTableDataObject.h"
#include "voUtils.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTree.h>

// --------------------------------------------------------------------------
// voOneZoom methods

// --------------------------------------------------------------------------
voOneZoom::voOneZoom():
    Superclass()
{
}

// --------------------------------------------------------------------------
voOneZoom::~voOneZoom()
{
}


// --------------------------------------------------------------------------
void voOneZoom::setOutputInformation()
{
  this->addOutputType("newickTree", "vtkTree",
                      "voOneZoomDynView", "One Zoom Visualization",
                      "", "");
}

// --------------------------------------------------------------------------
void voOneZoom::setParameterInformation()
{//no parameters
}

// --------------------------------------------------------------------------
QString voOneZoom::parameterDescription()const
{
 return "";
}

// --------------------------------------------------------------------------
bool voOneZoom::execute()
{
  voInputFileDataObject * obj = dynamic_cast<voInputFileDataObject*> (this->input(0));
  if (obj)
    {
     // Requires an input file type: feed the input file name to read in the newick
     // format tree into the javascript
    vtkTree* tree =  vtkTree::SafeDownCast(this->input(0)->dataAsVTKDataObject());
    QString filename =  dynamic_cast<voInputFileDataObject *>(this->input(0))->fileName();
    if (!tree)
      {
      qCritical() << "Input tree is Null";
      return false;
      }

    //output both the vtkTree and the newick tree file name
    voInputFileDataObject * outputDataObj = new voInputFileDataObject("newickTree", tree);
    outputDataObj->setFileName(filename);
    this->setOutput("newickTree", outputDataObj);
    }
   else
    {
    // TO-DO
    // Write out the output tree into a newick string and pass it to the OneZoom vis javascript
    }

  emit complete();
  return true;
}
