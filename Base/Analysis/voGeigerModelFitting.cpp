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
#include "voGeigerModelFitting.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTree.h>

// --------------------------------------------------------------------------
// voGeigerModelFittingPrivate methods

// --------------------------------------------------------------------------
class voGeigerModelFittingPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voGeigerModelFitting methods

// --------------------------------------------------------------------------
voGeigerModelFitting::voGeigerModelFitting():
    Superclass(), d_ptr(new voGeigerModelFittingPrivate)
{
  Q_D(voGeigerModelFitting);

  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}


// --------------------------------------------------------------------------
voGeigerModelFitting::~voGeigerModelFitting()
{
}


// --------------------------------------------------------------------------
void voGeigerModelFitting::setInputInformation()
{
  this->addInputType("input1", "vtkTree");
  this->addInputType("input2", "vtkTable");
}

// --------------------------------------------------------------------------
void voGeigerModelFitting::setOutputInformation()
{
  this->addOutputType("modelTree", "vtkTree" ,
                      "voTreeHeatmapView", "Fitted Model Tree");

}

// --------------------------------------------------------------------------
void voGeigerModelFitting::setParameterInformation()
{
  QList<QtProperty*> GeigerModel_parameters;

  QStringList modeling_methods;
  // Note: R supports additional methods not provided here
  modeling_methods << "OU" << "BM" << "lambda";
  GeigerModel_parameters << this->addEnumParameter("modelType", tr("Model Type"), modeling_methods, "OU");

  this->addParameterGroup("GeigerModel parameters", GeigerModel_parameters);
}

// --------------------------------------------------------------------------
QString voGeigerModelFitting::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Model Type</b>:</dt>"
                 "<dd>Geiger Tree Model Types.<br>"
                 "<u>Choose from the three existing models</u>,OU, BM, and lambda </dd>"
                 "</dl>");
}

// --------------------------------------------------------------------------
bool voGeigerModelFitting::execute()
{
  Q_D(voGeigerModelFitting);

  // Parameters
  QString modelType= this->enumParameter("modelType");

  // Import tree and assiciated traits table
  vtkTree* tree =  vtkTree::SafeDownCast(this->input("input1")->dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "Input 1 (tree) is Null";
    return false;
    }

  vtkTable* table =  vtkTable::SafeDownCast(this->input("input2")->dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "Input 2 (table) is Null";
    return false;
    }

  //table
  vtkNew<vtkArrayData> RInputArrayData;
  vtkSmartPointer<vtkArray> RInputArray;
  voUtils::tableToArray(table, RInputArray);
  RInputArrayData->AddArray(RInputArray.GetPointer());

  // Run R
  vtkNew <vtkRCalculatorFilter> RCalc;
  RCalc->RemoveAllPutVariables();
  RCalc->RemoveAllGetVariables();
  RCalc->SetRoutput(0);
  RCalc->AddInputData(0,tree);
  RCalc->AddInputData(0,table);
  RCalc->PutTree("tree");
  RCalc->PutTable("tableData");
  RCalc->GetTree("modelTree");

  if (modelType == "OU")
    {
    RCalc->SetRscript(QString(
        "library(geiger)\n"
        "svl<-as.numeric(tableData$SVL)\n"
        "names(svl)<-tree$tip.label \n"
        "o<-fitContinuous(tree, svl, model=\"%1\",bounds=list(alpha=c(1e-10,150)))\n"
        "tree_t=tree[1:4]\n" //ouTree() only takes input phylo tree without "node.label"
        "class(tree_t)=\"phylo\"\n" //so just recreate a phylo tree with the 5th element removed
        "modelTree<-ouTree(tree_t,alpha=o$opt$alpha)\n"
        ).arg(modelType).toLatin1());
    }


  if (modelType == "BM")
    {
    }

  if (modelType == "lamda")
    {
    }

  RCalc->Update();

  // Get R output
  vtkTree * outModelTree = vtkTree::SafeDownCast(RCalc->GetOutput());

  this->setOutput("modelTree", new voDataObject("modelTree",outModelTree));

  return true;
}
