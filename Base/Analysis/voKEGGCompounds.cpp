
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
void voKEGGCompounds::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voKEGGCompounds::setOutputInformation()
{
  this->addOutputType("analyte_IDs", "vtkTable",
                      "", "",
                      "voTableView", "Table (IDs)");

  this->addOutputType("analyte_pathways", "vtkTable",
                      "", "",
                      "voKEGGTableView", "Table (Pathways)");

  this->addOutputType("pathway_ranking", "vtkTable",
                      "", "",
                      "voKEGGTableView", "Pathway Ranking");
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
  vtkSmartPointer<vtkTable> inputTable = vtkSmartPointer<vtkTable>::Take(extendedTable->GetDataWithRowHeader());

  QList<QStringList> pathwaysList;
  int maxPathways = 0;

  //-------------------------------------------------------
  // Build Analyte ID Table
  vtkNew<vtkTable> analyteIDTable;
    {
    QUrl compoundURL(keggURL + "compound");
    for (vtkIdType rowCtr = 0; rowCtr < inputTable->GetNumberOfRows(); ++rowCtr)
      {
      compoundURL.addQueryItem("all", QString(inputTable->GetValue(rowCtr, 0).ToString()));
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

    if(responseSV.property("length").toInt32() != inputTable->GetNumberOfRows()) // Sanity check
      {
      qWarning() << "Error: Server returned" << responseSV.property("length").toInt32()
          << "results for" << inputTable->GetNumberOfRows() << "queries";
      return false;
      }
    for (vtkIdType rowCtr = 0; rowCtr < inputTable->GetNumberOfRows(); ++rowCtr)
      {
      if(QString::fromStdString(inputTable->GetValue(rowCtr, 0).ToString()) !=
         d->sValToStr(responseSV.property(rowCtr), "compound_name")) // Sanity check
        {
        qWarning() << "Error: Server returned out of order results";
        }
      QScriptValue compoundSV = responseSV.property(rowCtr);
      IDColumn->InsertNextValue(d->sValToStr(compoundSV, "compound_id").toStdString());
      titleColumn->InsertNextValue(d->sValToStrList(compoundSV, "compound_titles").join("; ").toStdString());
      pathwaysList << d->sValToStrPairList(compoundSV, "compound_pathways");
      maxPathways = qMax(maxPathways, pathwaysList.last().length());
      }

      analyteIDTable->AddColumn(inputTable->GetColumn(0));
      analyteIDTable->AddColumn(IDColumn.GetPointer());
      analyteIDTable->AddColumn(titleColumn.GetPointer());
    }
  this->setOutput("analyte_IDs", new voTableDataObject("analyte_IDs", analyteIDTable.GetPointer()));

  //-------------------------------------------------------
  // Build Analyte Pathways Table
  vtkNew<vtkTable> analytePathwaysTable;
    {
    analytePathwaysTable->AddColumn(inputTable->GetColumn(0));
    //analytePathwaysTable->AddColumn(IDColumn.GetPointer());
    for(int pathCtr = 0; pathCtr < maxPathways; pathCtr++)
      {
      vtkNew<vtkStringArray> pathwayColumn;
      pathwayColumn->SetName(QString("Pathway %1").arg(pathCtr + 1).toStdString().c_str());
      for (vtkIdType rowCtr = 0; rowCtr < inputTable->GetNumberOfRows(); ++rowCtr)
        {
        pathwayColumn->InsertNextValue(pathwaysList[rowCtr].value(pathCtr, "").toStdString());
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
    for (vtkIdType rowCtr = 0; rowCtr < inputTable->GetNumberOfRows(); ++rowCtr)
      {
      QString analyteName = QString(inputTable->GetValue(rowCtr, 0).ToString());
      foreach(QString pathwayID, pathwaysList[rowCtr])
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
      pathwayRankingTable->SetValue(rowCtr, 1, rawTexts.at(0).toLatin1().data());
      pathwayRankingTable->SetValue(rowCtr, 0, rawTexts.value(1, "").toLatin1().data());
      for(int colCtr = 0; colCtr < maxCount; colCtr++)
        {
        pathwayRankingTable->SetValue(rowCtr, colCtr+2, vtkVariant(maxLoc.value().at(colCtr).toLatin1().data()));
        }
      pathwayMap.erase(maxLoc);
      }
    }
  this->setOutput("pathway_ranking", new voTableDataObject("pathway_ranking", pathwayRankingTable.GetPointer()));

  return true;
}
