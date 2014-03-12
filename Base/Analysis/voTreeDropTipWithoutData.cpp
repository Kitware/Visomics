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
#include <QInputDialog>
#include <QMainWindow>
#include <QRegExp>

// Visomics includes
#include "voTreeDropTipWithoutData.h"
#include "voApplication.h"
#include "voOutputDataObject.h"
#include "voIOManager.h"
#include "voUtils.h"
#include "voTreeHeatmapView.h"


// VTK includes
#include <vtkArrayData.h>
#include <vtkIdTypeArray.h>
#include <vtkDendrogramItem.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkTree.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkExtractSelectedTree.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkDataSetAttributes.h>
#include <vtkInformation.h>
#include <vtkAdjacentVertexIterator.h>
#include <vtkTreeHeatmapItem.h>
// --------------------------------------------------------------------------
// voTreeDropTipWithoutDataPrivate methods

// --------------------------------------------------------------------------
class voTreeDropTipWithoutDataPrivate
{
};


// voTreeDropTipWithoutData methods

// --------------------------------------------------------------------------
voTreeDropTipWithoutData::voTreeDropTipWithoutData():
  Superclass(), d_ptr(new voTreeDropTipWithoutDataPrivate)
{
}

// --------------------------------------------------------------------------
voTreeDropTipWithoutData::~voTreeDropTipWithoutData()
{
}


// --------------------------------------------------------------------------
void voTreeDropTipWithoutData::setOutputInformation()
{
  this->addOutputType("pruned_tree", "vtkTree" ,
    "","",
    "voTreeHeatmapView", "pruned tree");
}

// --------------------------------------------------------------------------
void voTreeDropTipWithoutData::setParameterInformation()
{
  QList<QtProperty*> DropTip_parameters;

  DropTip_parameters << this->addEnumParameter("selection_method", tr("Select tips to be removed by"), (QStringList()<<"Tip Names"<<"SubTree Collapsing"), "Tip Names");
  DropTip_parameters << this->addStringParameter("input_string", tr("Line editor"), "ahli,alayoni,alfaroi");
  DropTip_parameters << this->addEnumParameter("invert_selection", tr("Invert selection"), (QStringList()<<"No"<<"Yes"), "No");

  this->addParameterGroup("DropTip parameters", DropTip_parameters);
}

// --------------------------------------------------------------------------
QString voTreeDropTipWithoutData::parameterDescription()const
{
  return QString("<dl>"
    "<dt><b>Select tips to be removed by: </b>:</dt>"
    "<dd>Choose the strategy for tip selection:"
    " \"Tip Names\" requires listing the tip "
    "names in the line editor below, separated by"
    " \",\"; \"SubTree Collapsing\" removes the collapsed "
    " tips (replaced by the triangle) from the viewer.</dd>"
    "<dt><b> Line editor </b>:</dt>"
    "<dd> Input the list of tips names, e.g. tipName1, tipName2, tipName3</dd>"
    "<dt><b>Invert selection </b>:</dt>"
    "<dd> Invert the tip selection. If \"Yes\", The selected tips are to be kept.</dd>"
    "</dl>");
}


// --------------------------------------------------------------------------
int voTreeDropTipWithoutData::execute()
{
  // Import tree
  vtkTree* inputTree =  vtkTree::SafeDownCast(this->input(0)->dataAsVTKDataObject());
  if (!inputTree)
    {
    qCritical() << "Input tree is Null";
    return voAnalysis::FAILURE;
    }


  vtkNew<vtkTree> tree;
  tree->DeepCopy(inputTree);
  // obtain selected tips into a vtkSelection object
  QString selection_method = this->enumParameter("selection_method");

  vtkNew<vtkSelection> selectedTips;
  bool SUCCESS = false;
  if (selection_method == "Tip Names")
    {
    SUCCESS = this->getSelectionByTipNames(tree.GetPointer(), selectedTips.GetPointer());
    }
  else if (selection_method == "Tree Collapsing")
    {
    SUCCESS = this->getSelectionByPrunedTree(tree.GetPointer(),selectedTips.GetPointer());
    }

  if (SUCCESS)
    {
 //   clock_t t;
 //   t = clock();
    vtkNew<vtkExtractSelectedTree> extractSelectedTreeFilter;

    extractSelectedTreeFilter->SetInputData(0, tree.GetPointer());
    extractSelectedTreeFilter->SetInputData(1, selectedTips.GetPointer());
    extractSelectedTreeFilter->Update();

    vtkTree * outTree = vtkTree::SafeDownCast(extractSelectedTreeFilter->GetOutput());
 //   t = clock()-t;
 //   qDebug()<<"It took " << ((float)t/CLOCKS_PER_SEC) <<" seconds).";

    if(!outTree)
      {
      qCritical() << QObject::tr("extracted tree is not valid");
      return voAnalysis::FAILURE;
      }

    unsigned int numFewerVertices =
      tree.GetPointer()->GetNumberOfVertices() - outTree->GetNumberOfVertices();
    qDebug() << "output tree has" << numFewerVertices
             << "fewer vertices than the input tree.";
    this->setOutput("pruned_tree", new voOutputDataObject("pruned_tree", outTree));

    return voAnalysis::SUCCESS;
    }
  else
    {
    qCritical() << QObject::tr("selected tips are not valid");
    return voAnalysis::FAILURE;
    }
}

// --------------------------------------------------------------------------
bool voTreeDropTipWithoutData::removeInternalBranch(vtkTree * tree, vtkIdTypeArray * selArray, vtkIdType currentId)
{// remove the internal branch of the selected tips
 //  by adding internal verticies/nodes to the selection
  if (tree->IsLeaf(currentId))
    {//do nothing
    return true;
    }

  vtkSmartPointer <vtkAdjacentVertexIterator> it  =
            vtkSmartPointer<vtkAdjacentVertexIterator>::New();
  tree->GetChildren(currentId,it);
  int numberOfSelectedChildren = 0;
  while (it->HasNext())
   {
   vtkIdType childId = it->Next();
   this->removeInternalBranch(tree, selArray,childId);
   //count the number of selected children
   for ( vtkIdType i = 0; i < selArray->GetNumberOfTuples(); i++)
     {
     if (selArray->GetValue(i) == childId)
       {
       numberOfSelectedChildren++;
       break;
       }
     }
   }

   if (numberOfSelectedChildren == tree->GetNumberOfChildren(currentId))
     {// add the current internal node to the selection list as well
     selArray->InsertNextValue(currentId);
     }

  return true;
}


// --------------------------------------------------------------------------
bool voTreeDropTipWithoutData::getTipSelection(vtkTree * tree, vtkSelection * sel, QStringList tipNameList)
{
  QStringList removalTipNameList;
  vtkStringArray * nodeNames = vtkStringArray::SafeDownCast(tree->GetVertexData()->GetAbstractArray("node name"));
  QString InvertSelection = this->enumParameter("invert_selection");
  if (InvertSelection == "No" )
    {
    removalTipNameList = tipNameList;
    }
  else
    {// input list is the keep list
    for ( vtkIdType i = 0; i< nodeNames->GetNumberOfValues() ; i++)
      {
      if (tree->IsLeaf(i))
        {
        QString curTip = QString(nodeNames->GetValue(i).c_str());
        if  ( !curTip.isEmpty() && !tipNameList.contains(curTip))
          {
          removalTipNameList.append(curTip);
          }
        }
      }
    }

  vtkSmartPointer<vtkSelectionNode> selNode = vtkSmartPointer <vtkSelectionNode>::New() ;
  vtkSmartPointer<vtkIdTypeArray> selArr = vtkSmartPointer<vtkIdTypeArray>::New();
  // extract subtree using vtkExtractSelectedTree class
  for (int i =0; i < removalTipNameList.size(); i++)
    {
    vtkIdType vertexId = nodeNames->LookupValue(removalTipNameList[i].toStdString().c_str());
    if (vertexId >= 0)
      {
      selArr->InsertNextValue(vertexId);
      }
    else
      {
      qWarning()<< QObject::tr("Could not find the tip names in the tree:") << removalTipNameList[i];
      }
    }

  //remove vertices whoes children are in the selection list recursively
  this->removeInternalBranch(tree,selArr,tree->GetRoot());

  selNode->SetContentType(vtkSelectionNode::INDICES);
  selNode->SetFieldType(vtkSelectionNode::VERTEX);
  selNode->SetSelectionList(selArr.GetPointer());

  selNode->GetProperties()->Set(vtkSelectionNode::INVERSE(), 1);
  sel->AddNode(selNode.GetPointer());

  return true;
}


// --------------------------------------------------------------------------
bool voTreeDropTipWithoutData::getSelectionByTipNames(vtkTree * tree, vtkSelection * sel)
{
  QString scratchString = this->stringParameter("input_string");
  QStringList tipNameList;
  QStringList removalTipNameList;
  if (!scratchString.isEmpty())
    {
    tipNameList = scratchString.split(",");
    }

  if(tipNameList.empty())
    {
    qCritical() << QObject::tr("Invalid paramater, could not parse input tip name list");
    return false;
    }

  return (this->getTipSelection(tree,sel, tipNameList));
}


// --------------------------------------------------------------------------
bool voTreeDropTipWithoutData::getSelectionByPrunedTree(vtkTree *tree, vtkSelection * sel)
{
  QStringList tipNameList;
  voTreeHeatmapView * view =  dynamic_cast<voTreeHeatmapView* > (this->getView());
  vtkTreeHeatmapItem * treeViewItem = NULL;
  vtkTree * prunedTree = NULL;
  if (view )
    {
    treeViewItem = view->getTreeHeatmapItem();
    prunedTree = treeViewItem->GetDendrogram()->GetPrunedTree();
    vtkStringArray * prunedNodeNames = vtkStringArray::SafeDownCast(prunedTree->GetVertexData()->GetAbstractArray("node name"));
    vtkStringArray * originalNodeNames = vtkStringArray::SafeDownCast(tree->GetVertexData()->GetAbstractArray("node name"));
    for ( vtkIdType i = 0; i< originalNodeNames->GetNumberOfValues(); ++i)
      {
      if (tree->IsLeaf(i))
        {
        if (prunedNodeNames->LookupValue(originalNodeNames->GetValue(i)) < 0 )
          {
          tipNameList.append(QString(originalNodeNames->GetValue(i).c_str()));
          }
        }
      }
    }

  return (getTipSelection(tree, sel, tipNameList));
}
