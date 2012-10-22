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
#include <QFile>
#include <QTextStream>


// Visomics includes
#include "voDataObject.h"
#include "voInputFileDataObject.h"
#include "voOneZoomDynView.h"
#include "voUtils.h"

// VTK includes
#include <vtkTree.h>

// --------------------------------------------------------------------------
class voOneZoomDynViewPrivate
{
public:
  voOneZoomDynViewPrivate();
};

// --------------------------------------------------------------------------
// voOneZoomDynViewPrivate methods

// --------------------------------------------------------------------------
voOneZoomDynViewPrivate::voOneZoomDynViewPrivate()
{
}

// --------------------------------------------------------------------------
// voOneZoomDynView methods

// --------------------------------------------------------------------------
voOneZoomDynView::voOneZoomDynView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voOneZoomDynViewPrivate)
{
}

// --------------------------------------------------------------------------
voOneZoomDynView::~voOneZoomDynView()
{
}

// --------------------------------------------------------------------------
QString voOneZoomDynView::stringify(const voDataObject& dataObject)
{
  vtkTree * tree = vtkTree::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "voOneZoomDynView - Failed to setDataObject - vtkTree data is expected !";
    return QString();
    }

  //Ideally, a vtkNewickTreeWriter function can be used to convert a tree to a newick formated string;
  //For now, we just use the original input file to obtain the newick string.
  QString filename =  dynamic_cast<voInputFileDataObject *> (const_cast<voDataObject*>(&dataObject))->fileName();

  //read the tree file into a string
  QFile file(filename);
  QString treeString = "";
  if (file.open (QIODevice::ReadOnly | QIODevice::Text))
    {
    QTextStream stream ( &file );
    treeString = stream.readAll();
    }
  file.close(); // when your done.


  return treeString;
}
