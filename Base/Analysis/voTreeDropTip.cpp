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
#include "voTreeDropTip.h"
#include "voApplication.h"
#include "voTableDataObject.h"
#include "voOutputDataObject.h"
#include "voIOManager.h"
#include "voUtils.h"


// VTK includes
#include <vtkArrayData.h>
#include <vtkIdTypeArray.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkExtendedTable.h>
#include <vtkExtractSelectedTree.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkDataSetAttributes.h>
#include <vtkInformation.h>
#include <vtkAdjacentVertexIterator.h>
// --------------------------------------------------------------------------
// voTreeDropTipPrivate methods

// --------------------------------------------------------------------------
class voTreeDropTipPrivate
{
};

// --------------------------------------------------------------------------
bool voTreeDropTip::removeInternalBranch(vtkTree * tree, vtkIdTypeArray * selArray, vtkIdType currentId)
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
bool voTreeDropTip::getTipSelection(vtkTree * tree, vtkTable * inputDataTable, vtkSelection * sel, QStringList tipNameList)
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
      QString curTip = QString(nodeNames->GetValue(i).c_str());
      if  ( !curTip.isEmpty() && !tipNameList.contains(curTip))
        {
        removalTipNameList.append(curTip);
        }
      }
    }

  vtkSmartPointer<vtkSelectionNode> selNode = vtkSmartPointer <vtkSelectionNode>::New() ;
  vtkSmartPointer<vtkIdTypeArray> selArr = vtkSmartPointer<vtkIdTypeArray>::New();
  // extract subtree using vtkExtractSelectedTree class
  // remove rows from the table
  vtkStringArray *tableNames = vtkStringArray::SafeDownCast(inputDataTable->GetColumn(0));
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
      return false;
      }

    vtkIdType rowId = tableNames->LookupValue(removalTipNameList[i].toStdString().c_str());
    if (rowId >= 0)
      {
      inputDataTable->RemoveRow(rowId);
      }
    else
      {
      qWarning()<< QObject::tr("Could not find the tip names in the table:") << removalTipNameList[i];
      return false;
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
bool voTreeDropTip::getSelectionByTipNames(vtkTree * tree, vtkTable * inputDataTable, vtkSelection * sel)
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
    qWarning() << QObject::tr("Invalid paramater, could not parse input tip name list");
    return false;
    }

  return (this->getTipSelection(tree, inputDataTable, sel, tipNameList));
}


// --------------------------------------------------------------------------
bool voTreeDropTip::getSelectionByDataFiltering(vtkTree *tree, vtkTable * inputDataTable, vtkSelection * sel)
{
  QStringList tipNameList;
  QString scratchString = this->stringParameter("input_string");
  QStringList conditionList;
  if ( !scratchString.isEmpty())
    {
    conditionList = scratchString.split(",");
    }


  if(conditionList.empty())
    {
    qWarning() << QObject::tr("Invalid paramater, could not parse input condition list");
    return false;
    }

  vtkStringArray *tableNames = vtkStringArray::SafeDownCast(inputDataTable->GetColumn(0));
  double epsilon = 1e-5;

  for (vtkIdType row = 0; row < inputDataTable->GetNumberOfRows(); row++)
    {// for each tip
    int numberOfConditionSatisfied = 0;
    for (int i = 0; i < conditionList.size(); i++)
      { //parse each condition
      QString condition = conditionList[i];
      bool conditionSatisfied = false;
      QRegExp rx("\\b(<|>|=)");
      int pos = rx.indexIn(condition, 0);
      std::string colName = condition.left(pos).toStdString();

      bool STRING_CONDITION = false;
      if (condition.count("\"") == 2)
        {
        STRING_CONDITION = true;
        }

      if (STRING_CONDITION)
        {// string comparison
        if (condition.at(pos) == QChar('='))
          {
          QString v = QString ((inputDataTable->GetValueByName(row,colName.c_str()).ToString()).c_str());
          QString  refString = condition.section('=',1,1);
          refString.chop(1);
          refString = refString.right(refString.size()-1);
          if ( v == refString )
            {
            conditionSatisfied = true;
            }
          }
        else
          {
          qCritical()<<"String filtering can only use \"=\"";
          return false;
          }
        }
      else
        {// number comparison
        double v = inputDataTable->GetValueByName(row,colName.c_str()).ToDouble();
        double threshold;
        if (condition.at(pos) == QChar('<'))
          {
          if (condition.at(pos+1) == QChar('='))
            { // <=
            threshold = condition.right(condition.size()-pos).toDouble();
            if (v <= threshold)
              {
              conditionSatisfied = true;
              }
            }
          else
            {// <
            threshold = condition.right(condition.size()-pos-1).toDouble();
            if (v < threshold)
              {
              conditionSatisfied = true;
              }
            }
          }
        else if (condition.at(pos) == QChar('>'))
          {
          if(condition.at(pos+1) == QChar('='))
            {// >=
            threshold = condition.right(condition.size()-pos-2).toDouble();
            if (v >= threshold)
              {
              conditionSatisfied = true;
              }
            }
          else
            {//>
            threshold = condition.right(condition.size()-pos-1).toDouble();
            if (v > threshold)
              {
              conditionSatisfied = true;
              }
            }
          }
        else if (condition.at(pos) == QChar('='))
          { //=
          threshold = condition.right(condition.size()-pos-1).toDouble();
          if ( fabs(v- threshold) < epsilon)
            {
            conditionSatisfied = true;
            }
          }
        else
          {
          qCritical()<<"Error parsing the conditions; please check your syntax.";
          return false;
          }
        }// end of numeric condition

      if (!conditionSatisfied)
        {
        break;
        }
      else
        {
        numberOfConditionSatisfied ++;
        }
      } //end of "for each condition"

    if (numberOfConditionSatisfied == conditionList.size())
      { // this tip meets all conditions
      // qDebug()<<"tip name:"<<QString(tableNames->GetValue(row).c_str());
      tipNameList.append(QString(tableNames->GetValue(row).c_str()));
      }
    } // end of "for each tip"
  // qDebug()<<"tipNameList"<<tipNameList;
  return (getTipSelection(tree,inputDataTable,sel,tipNameList));
}

// --------------------------------------------------------------------------
// voTreeDropTip methods

// --------------------------------------------------------------------------
voTreeDropTip::voTreeDropTip():
  Superclass(), d_ptr(new voTreeDropTipPrivate)
{
}

// --------------------------------------------------------------------------
voTreeDropTip::~voTreeDropTip()
{
}


// --------------------------------------------------------------------------
void voTreeDropTip::setOutputInformation()
{
  this->addOutputType("pruned_tree", "vtkTree" ,
    "","",
    "voTreeHeatmapView", "pruned tree");
  this->addOutputType("pruned_table", "vtkExtendedTable" ,
    "","",
    "voExtendedTableView", "pruned table");
}

// --------------------------------------------------------------------------
void voTreeDropTip::setParameterInformation()
{
  QList<QtProperty*> DropTip_parameters;

  DropTip_parameters << this->addEnumParameter("selection_method", tr("Select tips to be removed by"), (QStringList()<<"Tip Names"<<"Data Filter"), "Tip Names");
  DropTip_parameters << this->addStringParameter("input_string", tr("Line editor"), "[tipName1, tipName2] or [awesomeness<2,island=\"Cuba\"]");
  DropTip_parameters << this->addEnumParameter("invert_selection", tr("Invert selection"), (QStringList()<<"No"<<"Yes"), "No");

  this->addParameterGroup("DropTip parameters", DropTip_parameters);
}

// --------------------------------------------------------------------------
QString voTreeDropTip::parameterDescription()const
{
  return QString("<dl>"
    "<dt><b>Select tips to be removed by: </b>:</dt>"
    "<dd>Choose the method for the tip selection."
    " \"Tip Names\" requires you to list the tip "
    "names in the line editor below;\"Data Filter\" requires"
    " you to list the thresholding criteria in the line editor below.</dd>"
    "<dt><b> Line editor </b>:</dt>"
    "<dd> Input the tips names or the data filtering criteria.</dd>"
    "<dt><b>Invert selection </b>:</dt>"
    "<dd> Invert the tip selection. If \"Yes\", The selected tips are to be kept.</dd>"
    "</dl>");
}


// --------------------------------------------------------------------------
bool voTreeDropTip::execute()
{
  Q_D(voTreeDropTip);
  // Import tree and assiciated traits table
  vtkTree* tree =  vtkTree::SafeDownCast(this->input(0)->dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "Input tree is Null";
    return false;
    }

  vtkExtendedTable* extendedTable = vtkExtendedTable::SafeDownCast(
    this->input(1)->dataAsVTKDataObject());

  vtkNew<vtkTable> table;
  if (extendedTable)
    {
    table->DeepCopy(extendedTable->GetInputData());
    }
  else
    {
    qCritical()<<"Input Table is Null";
    }

  // obtain selected tips into a vtkSelection object
  QString selection_method = this->enumParameter("selection_method");

  vtkNew<vtkSelection> selectedTips;
  bool SUCCESS = false;
  if (selection_method == "Tip Names")
    {
    SUCCESS = this->getSelectionByTipNames(tree, table.GetPointer(), selectedTips.GetPointer());
    }
  else if (selection_method == "Data Filter")
    {
    SUCCESS = this->getSelectionByDataFiltering(tree, table.GetPointer(), selectedTips.GetPointer());
    }

  if (SUCCESS)
    {
    vtkNew<vtkExtractSelectedTree> extractSelectedTreeFilter;

    extractSelectedTreeFilter->SetInputData(0, tree);
    extractSelectedTreeFilter->SetInputData(1, selectedTips.GetPointer());
    extractSelectedTreeFilter->Update();

    vtkTree * outTree = vtkTree::SafeDownCast(extractSelectedTreeFilter->GetOutput());

    if(!outTree)
      {
      qCritical() << QObject::tr("extracted tree is not valid");
      return false;
      }

    this->setOutput("pruned_tree", new voOutputDataObject("pruned_tree", outTree));
    vtkNew<vtkExtendedTable> outputTable;
    voIOManager::convertTableToExtended(table.GetPointer(), outputTable.GetPointer());
    this->setOutput("pruned_table", new voTableDataObject("pruned_table", outputTable.GetPointer()));

    return true;
    }
  else
    {
    qCritical() << QObject::tr("selected tips are not valid");
    return false;
    }


}
