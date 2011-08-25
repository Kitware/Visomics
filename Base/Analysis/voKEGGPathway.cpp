
// Qt includes
#include <QDebug>
#include <QScriptValue>
#include <QUrl>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voDataObject.h"
#include "voKEGGPathway.h"
#include "voKEGGUtils.h"

// VTK includes
#include <vtkDataSetAttributes.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>

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
  // Not actually used, but voAnalysisDriver always expects exactly 1 input
}

// --------------------------------------------------------------------------
void voKEGGPathway::setOutputInformation()
{
  this->addOutputType("pathway_graph", "vtkGraph",
                      "voKEGGPathwayView", "Graph");

  this->addOutputType("pathway_map", "QPixmap",
                      "voKEGGImageView", "Map");
}

// --------------------------------------------------------------------------
void voKEGGPathway::setParameterInformation()
{
  QList<QtProperty*> kegg_parameters;

  // KEGG host
  kegg_parameters << this->addStringParameter("host", tr("Server name"), "paraviewweb.kitware.com:8090");
  kegg_parameters << this->addStringParameter("pathway_id", tr("Pathway ID"), "");
  this->addParameterGroup("KEGG parameters", kegg_parameters);
}

// --------------------------------------------------------------------------
bool voKEGGPathway::execute()
{
  //-------------------------------------------------------
  // Get and validate parameters
  QString keggURL("http://" + this->stringParameter("host") + "/kegg/");

  QString pathwayID =  this->stringParameter("pathway_id");
  if (!pathwayID.startsWith("path:"))
    {
    qWarning() << "Pathway ID does not start with \"path:\"";
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
  // TODO

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
