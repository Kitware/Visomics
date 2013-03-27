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
#include "voTreeDropTip.h"
#include "voTableDataObject.h"
#include "voOutputDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkIdTypeArray.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkExtractSelectedTree.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkDataSetAttributes.h>
#include <vtkInformation.h>
// --------------------------------------------------------------------------
// voTreeDropTipPrivate methods

// --------------------------------------------------------------------------
class voTreeDropTipPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voTreeDropTip methods

// --------------------------------------------------------------------------
voTreeDropTip::voTreeDropTip():
    Superclass(), d_ptr(new voTreeDropTipPrivate)
{
  Q_D(voTreeDropTip);

  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}

// --------------------------------------------------------------------------
voTreeDropTip::~voTreeDropTip()
{
}


// --------------------------------------------------------------------------
void voTreeDropTip::setOutputInformation()
{
  this->addOutputType("newtree", "vtkTree" ,
                       "","",
                      "voTreeHeatmapView", "Extracted Sub Tree");
  //this->addOutputType("newtable", "vtkExtendedTable" ,
  //                     "","",
  //                    "voTableView", "Extracted Sub Table");
}

// --------------------------------------------------------------------------
void voTreeDropTip::setParameterInformation()
{
  QList<QtProperty*> DropTip_parameters;

  DropTip_parameters << this->addStringParameter("tipNames", QObject::tr("Tips to be removed"), "tipName1, tipName2");
  //DropTip_parameters << this->addStringParameter("data name", QObject::tr("Response Analyte(s)"), "4,5,7-10");

  this->addParameterGroup("DropTip parameters", DropTip_parameters);
}

// --------------------------------------------------------------------------
QString voTreeDropTip::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Tips to be removed </b>:</dt>"
                 "<dd>A list of tip node names.</dd>"
                 "</dl>");
}

// --------------------------------------------------------------------------
bool voTreeDropTip::execute()
{
  // Import tree and assiciated traits table
  vtkTree* tree =  vtkTree::SafeDownCast(this->input(0)->dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "Input tree is Null";
    return false;
    }

  vtkExtendedTable* extendedTable =
    vtkExtendedTable::SafeDownCast(this->input(1)->dataAsVTKDataObject());
  if (extendedTable)
    {
    vtkTable * table = extendedTable->GetInputData();
    if (!table)
      {
      qCritical() << "Input 2 (table) is Null";
      return false;
      }
    }
   else
    {
    qCritical()<<"Input Table is Null";
    }


  // Get and parse parameters
  QString scratchString = this->stringParameter("tipNames");
  QStringList  tipNameList = scratchString.split(",");
  if(tipNameList.empty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse input tip name list");
    return false;
    }

  // extract subtree using vtkExtractSelectedTree class
   vtkNew<vtkSelection> sel;
   vtkNew<vtkSelectionNode> selNode;
   vtkNew<vtkIdTypeArray> selArr;
   vtkStringArray * nodeNames = vtkStringArray::SafeDownCast(tree->GetVertexData()->GetAbstractArray("node name"));
   for (int i =0; i < tipNameList.size(); i++)
    {
    vtkIdType vertexId = nodeNames->LookupValue(tipNameList[i].toStdString().c_str());
    if (vertexId >= 0)
      {
      selArr->InsertNextValue(vertexId);
      }
    else
      {
      qWarning()<< QObject::tr("Could not find the tip names:") << tipNameList[i];
      return false;
      }
    }
   selNode->SetContentType(vtkSelectionNode::INDICES);
   selNode->SetFieldType(vtkSelectionNode::VERTEX);
   selNode->SetSelectionList(selArr.GetPointer());
   selNode->GetProperties()->Set(vtkSelectionNode::INVERSE(), 1);
   sel->AddNode(selNode.GetPointer());

  vtkNew<vtkExtractSelectedTree> extractSelectedTreeFilter;
  extractSelectedTreeFilter->SetInputData(0, tree);
  extractSelectedTreeFilter->SetInputData(1, sel.GetPointer());
  extractSelectedTreeFilter->Update();

  vtkTree * outTree = vtkTree::SafeDownCast(extractSelectedTreeFilter->GetOutput());
  if(!outTree)
    {
    qCritical() << QObject::tr("extracted tree is not valid");
    return false;
    }
  this->setOutput("newtree", new voOutputDataObject("newtree", outTree));

  return true;
}
