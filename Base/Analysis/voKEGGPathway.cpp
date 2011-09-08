
// Qt includes
#include <QDebug>
#include <QHash>
#include <QQueue>
#include <QScriptValue>
#include <QUrl>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voConfigure.h"
#include "voDataObject.h"
#include "vtkExtendedTable.h"
#include "voKEGGPathway.h"
#include "voKEGGUtils.h"
#include "voTableDataObject.h"

// VTK includes
#include <vtkAdjacentVertexIterator.h>
#include <vtkDataSetAttributes.h>
#include <vtkIdTypeArray.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voKEGGPathwayPrivate methods

// --------------------------------------------------------------------------
class voKEGGPathwayPrivate
{
public:
};


// --------------------------------------------------------------------------
// voKEGGPathway methods

// --------------------------------------------------------------------------
voKEGGPathway::voKEGGPathway():
    Superclass(), d_ptr(new voKEGGPathwayPrivate)
{
  //Q_D(voKEGGPathway);
}

// --------------------------------------------------------------------------
voKEGGPathway::~voKEGGPathway()
{
}

// --------------------------------------------------------------------------
void voKEGGPathway::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voKEGGPathway::setOutputInformation()
{
  this->addOutputType("pathway_graph", "vtkGraph",
                      "voKEGGPathwayView", "Graph");

  this->addOutputType("pathway_shortest", "vtkTable",
                      "", "",
                      "voTableView", "Shortest Paths");

  this->addOutputType("pathway_map", "QPixmap",
                      "voKEGGImageView", "Map");
}

// --------------------------------------------------------------------------
void voKEGGPathway::setParameterInformation()
{
  QList<QtProperty*> kegg_parameters;

  // KEGG host
  kegg_parameters << this->addStringParameter("pathway_id", tr("Pathway ID"), "path:ko00010"); // Default is "Glycolysis / Gluconeogenesis"
  this->addParameterGroup("KEGG parameters", kegg_parameters);
}

// --------------------------------------------------------------------------
QString voKEGGPathway::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Pathway ID</b>:</dt>"
                 "<dd>A KEGG Orthology (KO) pathway ID.<br>"
                 "The <i>KEGG Compounds</i> analysis can provide pathway IDs and "
                 "launch a <i>KEGG Pathway</i> analysis directly.</dd>"
                 "</dl>");
}

// --------------------------------------------------------------------------
bool voKEGGPathway::execute()
{
  //-------------------------------------------------------
  // Get and validate parameters
  QString keggURL = QString("http://%1:%2/kegg/").arg(Visomics_KEGG_SERVER_HOSTNAME).arg(Visomics_KEGG_SERVER_PORT);

  QString pathwayID =  this->stringParameter("pathway_id");
  if (!pathwayID.startsWith("path:"))
    {
    qWarning() << "Pathway ID does not start with \"path:\"";
    return false;
    }

  // Import data table locally
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qWarning() << "Input is Null";
    return false;
    }

  //-------------------------------------------------------
  // Build vtkGraph of pathway
  vtkNew<vtkMutableDirectedGraph> graph;
    {
    graph->GetVertexData()->SetPedigreeIds(vtkSmartPointer<vtkStringArray>::New().GetPointer());
    graph->GetVertexData()->GetPedigreeIds()->SetName("PedigreeIds");

    QUrl graphURL(keggURL + "graph");
    graphURL.addQueryItem("path", pathwayID);

    QByteArray responseData;
    if(!voKEGGUtils::queryServer(graphURL, &responseData))
      {
      // Error message already printed within voKEGGUtils::queryServer()
      return false;
      }

    QScriptValue rawScriptValue;
    if(!voKEGGUtils::dataToJSON(responseData, &rawScriptValue))
      {
      // Error message already printed within voKEGGUtils::dataToJSON()
      return false;
      }

    QScriptValue pathwayScriptValue = rawScriptValue.property(0); // Only made 1 query
    for(int i = 0; i < pathwayScriptValue.property("pathway_graph").property("length").toInt32(); i++)
      {
      vtkStdString inVertexName = pathwayScriptValue.property("pathway_graph").property(i).property(0).toString().toStdString();
      vtkStdString outVertexName = pathwayScriptValue.property("pathway_graph").property(i).property(1).toString().toStdString();
      vtkIdType inVertexIndex = graph->AddVertex(vtkVariant(inVertexName));
      vtkIdType outVertexIndex = graph->AddVertex(vtkVariant(outVertexName));
      graph->AddEdge(inVertexIndex, outVertexIndex);
      }
    }
  this->setViewPrettyName("pathway_graph", "voKEGGPathwayView", QString("Graph (%1)").arg(this->stringParameter("pathway_id")));
  this->setOutput("pathway_graph", new voDataObject("pathway_graph", graph.GetPointer()));

  //-------------------------------------------------------
  // Find shortest paths between known analytes in graph
  vtkNew<vtkTable> shortestPathTable;
    {
    // Note: this implementation is for demo / proof of concept purposes.
    // To generate presentAnalytes, tt must refetch all analyte information
    // from the server, because there is no way to share data from voKEGGCompounds
    // (which should have already been run, and which will already fetch and
    // generate this same information)
    vtkNew<vtkStringArray> presentAnalyteNames;
    vtkNew<vtkStringArray> presentAnalytesIds;
    vtkNew<vtkIdTypeArray> presentAnalyteVertexIds;
      {
      vtkStringArray* analyteNames = extendedTable->GetRowMetaDataOfInterestAsString();
      QUrl compoundURL(keggURL + "compound");
      for (vtkIdType ctr = 0; ctr < analyteNames->GetNumberOfValues(); ++ctr)
        {
        compoundURL.addQueryItem("path", QString(analyteNames->GetValue(ctr)));
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
      for (vtkIdType ctr = 0; ctr < analyteNames->GetNumberOfValues(); ++ctr)
        {
        QScriptValue compoundSV = responseSV.property(ctr);
        if(QString::fromStdString(analyteNames->GetValue(ctr)) !=
           compoundSV.property("compound_name").toString()) // Sanity check
          {
          qWarning() << "Error: Server returned out of order results";
          return false;
          }
        for(int pathCtr = 0; pathCtr < compoundSV.property("compound_pathways").property("length").toInt32(); pathCtr++)
          {
          QString compoundPathId = compoundSV.property("compound_pathways").property(pathCtr).property(0).toString();
          if (compoundPathId == pathwayID) // The compound is included in this pathway
            {
            vtkVariant analyteId(compoundSV.property("compound_id").toString().toStdString());
            vtkIdType vertexId = graph->FindVertex(analyteId);
            if(vertexId != -1) // Some KEGG compounds may claim to be in a pathway, but have no actual vertex present
              {
              presentAnalyteNames->InsertNextValue(analyteNames->GetValue(ctr));
              presentAnalytesIds->InsertNextValue(analyteId.ToString());
              presentAnalyteVertexIds->InsertNextValue(vertexId);
              }
            break;
            }
          }
        }
      } // End presentAnalytes generation

    shortestPathTable->AddColumn(presentAnalyteNames.GetPointer());
    for(vtkIdType col = 0; col < presentAnalyteNames->GetNumberOfValues(); col++)
      {
      vtkNew<vtkStringArray> tempColumn;
      tempColumn->SetNumberOfValues(presentAnalyteNames->GetNumberOfValues());
      tempColumn->SetName(presentAnalyteNames->GetValue(col));
      shortestPathTable->AddColumn(tempColumn.GetPointer());
      }

    for(vtkIdType startCtr = 0; startCtr < presentAnalytesIds->GetNumberOfValues(); startCtr++)
      {
      vtkIdType startVertex = presentAnalyteVertexIds->GetValue(startCtr);

      // BFS to find distances (don't need Dijkstra's since edge weights are uniform)
      QHash<vtkIdType, double> distances;
      distances.insert(startVertex, 0.0);

      QQueue<vtkIdType> toVisit;
      toVisit.enqueue(startVertex);

      vtkNew<vtkAdjacentVertexIterator> adjItr;
      while(!toVisit.empty())
        {
        vtkIdType current = toVisit.dequeue();
        graph->GetAdjacentVertices(current, adjItr.GetPointer());
        while(adjItr->HasNext())
          {
          vtkIdType adj = adjItr->Next();
          if(!distances.contains(adj))
            {
            // Use only half distance, as all compounds are seperated by a reaction
            distances.insert(adj, (distances.value(current) + 0.5));
            toVisit.enqueue(adj);
            }
          }
        }

      for(vtkIdType endCtr = 0; endCtr < presentAnalytesIds->GetNumberOfValues(); endCtr++)
        {
        vtkIdType endVertex = presentAnalyteVertexIds->GetValue(endCtr);
        vtkVariant distanceString;
        if(distances.contains(endVertex))
          {
          distanceString = static_cast<int>(distances.value(endVertex));
          }
        else
          {
          distanceString = "-";
          }
        shortestPathTable->SetValue(startCtr, endCtr+1, distanceString);
        }
      }
    }
  this->setOutput("pathway_shortest", new voTableDataObject("pathway_shortest", shortestPathTable.GetPointer()));

  //-------------------------------------------------------
  // Fetch pathway map image
  QPixmap pixmap;
    {
    QUrl mapURL(keggURL + "map");
    mapURL.addQueryItem("path", pathwayID);

    QByteArray responseData;
    if(!voKEGGUtils::queryServer(mapURL, &responseData))
      {
      // Error message already printed within voKEGGUtils::queryServer()
      return false;
      }

    if (!pixmap.loadFromData(responseData))
      {
      qWarning() << "Could not load PNG image";
      return false;
      }
    }
  this->setViewPrettyName("pathway_map", "voKEGGImageView",QString("Map (%1)").arg(this->stringParameter("pathway_id")));
  this->setOutput("pathway_map", new voDataObject("pathway_map", static_cast<QVariant>(pixmap)));

  //-------------------------------------------------------
  return true;
}
