
// Qt includes
#include <QDebug>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisFactory.h"
#include "voQObjectFactory.h"

#include "voFoldChange.h"
#include "voHierarchicalClustering.h"
#include "voKEGG.h"
#include "voKMeansClustering.h"
#include "voPCAStatistics.h"
#include "voTTest.h"
#include "voXCorrel.h"
#include "voPLSStatistics.h"

//----------------------------------------------------------------------------
class voAnalysisFactoryPrivate
{
public:
  voQObjectFactory<voAnalysis> AnalysisFactory;
  QHash<QString, QString>      PrettyNameToNameMap;
  QHash<QString, QString>      NameToPrettyNameMap;
};

//----------------------------------------------------------------------------
// voAnalysisFactoryPrivate methods

//----------------------------------------------------------------------------
// voAnalysisFactory methods

//----------------------------------------------------------------------------
voAnalysisFactory::voAnalysisFactory():d_ptr(new voAnalysisFactoryPrivate)
{
  this->registerAnalysis<voFoldChange>("Fold Change");
  this->registerAnalysis<voHierarchicalClustering>("Hierarchical Clustering");
  this->registerAnalysis<voKEGG>("KEGG Pathways");
  this->registerAnalysis<voKMeansClustering>("KMeans Clustering");
  this->registerAnalysis<voPCAStatistics>("PCA");
  this->registerAnalysis<voTTest>("T-Test");
  this->registerAnalysis<voXCorrel>("Cross Correlation");
  this->registerAnalysis<voPLSStatistics>("PLS");
}


//-----------------------------------------------------------------------------
voAnalysisFactory::~voAnalysisFactory()
{
}

//-----------------------------------------------------------------------------
voAnalysis* voAnalysisFactory::createAnalysis(const QString& className)
{
  Q_D(voAnalysisFactory);
  voAnalysis * analysis = d->AnalysisFactory.Create(className);
  if (analysis)
    {
    analysis->setObjectName(d->NameToPrettyNameMap.value(className));
    }
  return analysis;
}

//-----------------------------------------------------------------------------
QStringList voAnalysisFactory::registeredAnalysisNames() const
{
  Q_D(const voAnalysisFactory);
  return d->AnalysisFactory.registeredObjectKeys();
}

//-----------------------------------------------------------------------------
QStringList voAnalysisFactory::registeredAnalysisPrettyNames() const
{
   Q_D(const voAnalysisFactory);
  return d->PrettyNameToNameMap.keys();
}

//-----------------------------------------------------------------------------
QString voAnalysisFactory::analysisNameFromPrettyName(const QString& analysisPrettyName) const
{
  Q_D(const voAnalysisFactory);
  return d->PrettyNameToNameMap.value(analysisPrettyName);
}

//-----------------------------------------------------------------------------
template<typename AnalysisClassType>
void voAnalysisFactory::registerAnalysis(const QString& analysisPrettyName)
{
  Q_D(voAnalysisFactory);
  
  if (analysisPrettyName.isEmpty())
    {
    qCritical() << "Failed to register analysis - analysisPrettyName is an empty string";
    return;
    }

  if (this->registeredAnalysisPrettyNames().contains(analysisPrettyName))
    {
    return;
    }

  QString analysisName = AnalysisClassType::staticMetaObject.className();

  if (d->AnalysisFactory.registeredObjectKeys().contains(analysisName))
    {
    return;
    }

  d->PrettyNameToNameMap.insert(analysisPrettyName, analysisName);
  d->NameToPrettyNameMap.insert(analysisName, analysisPrettyName);

  d->AnalysisFactory.registerObject<AnalysisClassType>(analysisName);
}
