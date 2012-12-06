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
#include <QScriptValue>
#include <QUrl>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voConfigure.h"
#include "voKEGGCompounds.h"
#include "voKEGGUtils.h"
#include "voTableDataObject.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayToTable.h>
#include <vtkDoubleArray.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voKEGGCompoundsPrivate methods

// --------------------------------------------------------------------------
class voKEGGCompoundsPrivate
{
public:

  QString sValToStr(QScriptValue sVal, QString property);
  QStringList sValToStrList(QScriptValue sVal, QString property);
  QStringList sValToStrPairList(QScriptValue sVal, QString property);
};

// --------------------------------------------------------------------------
QString voKEGGCompoundsPrivate::sValToStr(QScriptValue sVal, QString property)
{
  return sVal.property(property).toString();
}

// --------------------------------------------------------------------------
QStringList voKEGGCompoundsPrivate::sValToStrList(QScriptValue sVal, QString property)
{
  QStringList returnVal;
  for(int i = 0; i < sVal.property(property).property("length").toInt32(); i++)
    {
    returnVal << sVal.property(property).property(i).toString();
    }
  return returnVal;
}

// --------------------------------------------------------------------------
// Should eventually remove this, once voKEGGTableView can accept multiple data structures as input
QStringList voKEGGCompoundsPrivate::sValToStrPairList(QScriptValue sVal, QString property)
{
  QStringList returnVal;
  for(int i = 0; i < sVal.property(property).property("length").toInt32(); i++)
    {
    returnVal << (sVal.property(property).property(i).property(0).toString()
                  + "#" +
                  sVal.property(property).property(i).property(1).toString());
    }
  return returnVal;
}

// --------------------------------------------------------------------------
// voKEGGCompounds methods

// --------------------------------------------------------------------------
voKEGGCompounds::voKEGGCompounds():
    Superclass(), d_ptr(new voKEGGCompoundsPrivate)
{
  //Q_D(voKEGGCompounds);
}

// --------------------------------------------------------------------------
voKEGGCompounds::~voKEGGCompounds()
{
}

// --------------------------------------------------------------------------
void voKEGGCompounds::setOutputInformation()
{
  this->addOutputType("analyte_IDs", "vtkTable",
                      "", "",
                      "voTableView", "KEGG IDs");

  this->addOutputType("analyte_pathways", "vtkTable",
                      "", "",
                      "voKEGGTableView", "Pathway List");

  this->addOutputType("pathway_ranking", "vtkTable",
                      "", "",
                      "voKEGGTableView", "Ranking - Pathways");
}

// --------------------------------------------------------------------------
bool voKEGGCompounds::execute()
{
  Q_D(voKEGGCompounds);

  //-------------------------------------------------------
  // Get parameters
  QString keggURL = QString("http://%1:%2/kegg/").arg(Visomics_KEGG_SERVER_HOSTNAME).arg(Visomics_KEGG_SERVER_PORT);

  // Import data table locally
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qWarning() << "Input is Null";
    return false;
    }
  vtkStringArray* analyteNames = extendedTable->GetRowMetaDataOfInterestAsString();

  QList<QStringList> pathwaysList;
  int maxPathways = 0;

  //-------------------------------------------------------
  // Build Analyte ID Table
  vtkNew<vtkTable> analyteIDTable;
    {
    QUrl compoundURL(keggURL + "compound");
    for (vtkIdType ctr = 0; ctr < analyteNames->GetNumberOfValues(); ++ctr)
      {
      compoundURL.addQueryItem("all", QString(analyteNames->GetValue(ctr)));
      }

    QByteArray responseData;
    if(!voKEGGUtils::queryServer(compoundURL, &responseData))
      {
      // Error message already printed within voKEGGUtils::queryServer()
      return false;
      }

    QScriptValue responseSV;
    if(!voKEGGUtils::dataToJSON(responseData, &responseSV))
      {
      // Error message already printed within voKEGGUtils::dataToJSON()
      return false;
      }

    vtkNew<vtkStringArray> IDColumn;
    IDColumn->SetName("KEGG ID");
    vtkNew<vtkStringArray> titleColumn;
    titleColumn->SetName("KEGG Names");

    if(responseSV.property("length").toInt32() != analyteNames->GetNumberOfValues()) // Sanity check
      {
      qWarning() << "Error: Server returned" << responseSV.property("length").toInt32()
          << "results for" << analyteNames->GetNumberOfValues() << "queries";
      return false;
      }
    for (vtkIdType ctr = 0; ctr < analyteNames->GetNumberOfValues(); ++ctr)
      {
      QScriptValue compoundSV = responseSV.property(ctr);
      if(QString::fromStdString(analyteNames->GetValue(ctr)) !=
         d->sValToStr(compoundSV, "compound_name")) // Sanity check
        {
        qWarning() << "Error: Server returned out of order results";
        return false;
        }
      IDColumn->InsertNextValue(d->sValToStr(compoundSV, "compound_id").toStdString());
      titleColumn->InsertNextValue(d->sValToStrList(compoundSV, "compound_titles").join("; ").toStdString());
      QStringList compoundPathwayList = d->sValToStrPairList(compoundSV, "compound_pathways");
      for(int i = 0; i < compoundPathwayList.length();)
        {
        if(!compoundPathwayList.at(i).startsWith("path:ko"))
          {
          compoundPathwayList.removeAt(i);
          }
        else
          {
          i++;
          }
        }
      pathwaysList << compoundPathwayList;
      maxPathways = qMax(maxPathways, pathwaysList.last().length());
      }

      analyteIDTable->AddColumn(analyteNames);
      analyteIDTable->AddColumn(IDColumn.GetPointer());
      analyteIDTable->AddColumn(titleColumn.GetPointer());
    }
  this->setOutput("analyte_IDs", new voTableDataObject("analyte_IDs", analyteIDTable.GetPointer()));

  //-------------------------------------------------------
  // Build Analyte Pathways Table
  vtkNew<vtkTable> analytePathwaysTable;
    {
    analytePathwaysTable->AddColumn(analyteNames);
    //analytePathwaysTable->AddColumn(IDColumn.GetPointer());
    for(int pathCtr = 0; pathCtr < maxPathways; pathCtr++)
      {
      vtkNew<vtkStringArray> pathwayColumn;
      pathwayColumn->SetName(QString("Pathway %1").arg(pathCtr + 1).toStdString().c_str());
      for (vtkIdType ctr = 0; ctr < analyteNames->GetNumberOfValues(); ++ctr)
        {
        pathwayColumn->InsertNextValue(pathwaysList[ctr].value(pathCtr, "").toStdString());
        }
      analytePathwaysTable->AddColumn(pathwayColumn.GetPointer());
      }
    }
  this->setOutput("analyte_pathways", new voTableDataObject("analyte_pathways", analytePathwaysTable.GetPointer()));

  //-------------------------------------------------------
  // Build Pathway Ranking Table
  vtkNew<vtkTable> pathwayRankingTable;
    {
    QMap<QString, QStringList> pathwayMap;
    for (vtkIdType ctr = 0; ctr < analyteNames->GetNumberOfValues(); ++ctr)
      {
      QString analyteName(analyteNames->GetValue(ctr));
      foreach(QString pathwayID, pathwaysList[ctr])
        {
        pathwayMap[pathwayID].append(analyteName);
        }
      }
    int maxAnalytes = 0;
    // Remove all analytes with no common pathways
    for(QMap<QString, QStringList>::iterator mapItr = pathwayMap.begin(); mapItr != pathwayMap.end();)
      {
      int listLength = mapItr.value().size();
      maxAnalytes = qMax(listLength, maxAnalytes);
      if (listLength < 2)
        {
        mapItr = pathwayMap.erase(mapItr);
        }
      else
        {
        mapItr++;
        }
      }
    // Build table, with an extra 2 columns at the left for Title / Pathway ID
    for(int analyteCtr = -2; analyteCtr < maxAnalytes; analyteCtr++)
      {
      vtkNew<vtkStringArray> analyteColumn;
      analyteColumn->SetName(QString("Analyte %1").arg(analyteCtr+1).toStdString().c_str());
      analyteColumn->SetNumberOfValues(pathwayMap.size());
      pathwayRankingTable->AddColumn(analyteColumn.GetPointer());
      }
    pathwayRankingTable->GetColumn(0)->SetName("");
    pathwayRankingTable->GetColumn(1)->SetName("Pathway ID");
    // Extract common pathways in descending order; this takes O(n^2) time, where n = analytes;
    // we could do it in O(n*log(n)), but it will require additional code complexity and memory usage
    for(int rowCtr = 0; rowCtr < pathwayMap.size(); rowCtr++)
      {
      int maxCount = 0;
      QMap<QString, QStringList>::iterator maxLoc;
      for(QMap<QString, QStringList>::iterator mapItr = pathwayMap.begin(); mapItr != pathwayMap.end(); mapItr++)
        {
        if (mapItr.value().size() > maxCount)
          {
          maxCount = mapItr.value().size();
          maxLoc = mapItr;
          }
        }
      QStringList rawTexts = maxLoc.key().split("#");
      pathwayRankingTable->SetValue(rowCtr, 1, vtkVariant(rawTexts.at(0).toStdString()));
      pathwayRankingTable->SetValue(rowCtr, 0, vtkVariant(rawTexts.value(1, "").toStdString()));
      for(int colCtr = 0; colCtr < maxCount; colCtr++)
        {
        pathwayRankingTable->SetValue(rowCtr, colCtr+2, vtkVariant(maxLoc.value().at(colCtr).toStdString()));
        }
      pathwayMap.erase(maxLoc);
      }
    }
  this->setOutput("pathway_ranking", new voTableDataObject("pathway_ranking", pathwayRankingTable.GetPointer()));

  return true;
}
