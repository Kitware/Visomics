
// Qt includes
#include <QDebug>
#include <QList>
#include <QScriptEngine>
#include <QScriptValue>

// Visomics includes
#include "voDataObject.h"
#include "voKMeansClusteringDynView.h"
#include "voUtils.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voKMeansClusteringDynViewPrivate
{
public:
  voKMeansClusteringDynViewPrivate();
};

// --------------------------------------------------------------------------
// voKMeansClusteringDynViewPrivate methods

// --------------------------------------------------------------------------
voKMeansClusteringDynViewPrivate::voKMeansClusteringDynViewPrivate()
{
}

// --------------------------------------------------------------------------
// voKMeansClusteringDynView methods

// --------------------------------------------------------------------------
voKMeansClusteringDynView::voKMeansClusteringDynView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voKMeansClusteringDynViewPrivate)
{
}

// --------------------------------------------------------------------------
voKMeansClusteringDynView::~voKMeansClusteringDynView()
{
}

namespace
{

// --------------------------------------------------------------------------
QScriptValue scriptValueFromCluster(QScriptEngine* scriptEngine, vtkTable * clusterTable, int clusterId)
{
  QScriptValue clusterChildren = scriptEngine->newArray();
  quint32 childCount = 0;
  for(vtkIdType cid = 1; cid < clusterTable->GetNumberOfColumns() ; ++cid)
    {
    vtkIntArray * currentColumn = vtkIntArray::SafeDownCast(clusterTable->GetColumn(cid));
    int currentValue = currentColumn->GetValue(0);
    if (currentValue == clusterId)
      {
      QScriptValue clusterChild = scriptEngine->newObject();
      clusterChild.setProperty("name", QScriptValue(currentColumn->GetName()));
      clusterChildren.setProperty(childCount, clusterChild);
      ++childCount;
      }
    }
  return clusterChildren;
}

// --------------------------------------------------------------------------
QScriptValue scriptValueFromClusters(QScriptEngine* scriptEngine, vtkTable * clusterTable, quint32 kmeansCenters)
{
  QScriptValue array = scriptEngine->newArray();
  QScriptValueList::const_iterator it;
  for (quint32 i = 0; i < kmeansCenters; ++i)
    {
    QScriptValue cluster = scriptEngine->newObject();
    cluster.setProperty("name", QScriptValue(QString("Cluster %1").arg(i + 1)));
    cluster.setProperty("children", scriptValueFromCluster(scriptEngine, clusterTable, i + 1));
    array.setProperty(i, cluster);
    }
  return array;
}

} // end of anonymous namespace

// --------------------------------------------------------------------------
QString voKMeansClusteringDynView::stringify(const voDataObject& dataObject)
{
  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voKMeansClusteringDynView - Failed to setDataObject - vtkTable data is expected !";
    return QString();
    }

  bool ok = false;
  int kmeansCenters = dataObject.property("kmeans_centers").toInt(&ok);
  if (!ok)
    {
    qCritical() << "voKMeansClusteringDynView - Failed to setDataObject - 'kmeans_center' property is expected !";
    return QString();
    }

  QScriptEngine scriptEngine;
  QScriptValue object = scriptEngine.newObject();
  object.setProperty("name", QScriptValue("Clusters"));
  object.setProperty("children", scriptValueFromClusters(&scriptEngine, table, kmeansCenters));

  return voUtils::stringify(&scriptEngine, object);
}
