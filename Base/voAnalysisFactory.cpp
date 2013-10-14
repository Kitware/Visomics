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

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisFactory.h"
#include "voQObjectFactory.h"

#include "voANOVAStatistics.h"
#include "voFoldChange.h"
#include "voHierarchicalClustering.h"
#include "voKEGGCompounds.h"
#include "voKEGGPathway.h"
#include "voKMeansClustering.h"
#include "voOneZoom.h"
#include "voPCAStatistics.h"
#include "voPLSStatistics.h"
#include "voTTest.h"
#include "voXCorrel.h"
#include "voGeigerModelFitting.h"
#include "voTreeDropTip.h"
#include "voTreeDropTipWithoutData.h"
#include "voCompareTrees.h"

#include "voCustomAnalysis.h"
#include "voRemoteCustomAnalysis.h"
#include "voCustomAnalysisInformation.h"

//----------------------------------------------------------------------------
class voAnalysisFactoryPrivate
{
public:
  voQObjectFactory<voAnalysis> AnalysisFactory;
  QHash<QString, QString>      PrettyNameToNameMap;
  QHash<QString, QString>      NameToPrettyNameMap;
  QHash<QString, voCustomAnalysisInformation*> customAnalysisNameToInfoMap;
};

//----------------------------------------------------------------------------
// voAnalysisFactoryPrivate methods

//----------------------------------------------------------------------------
// voAnalysisFactory methods

//----------------------------------------------------------------------------
voAnalysisFactory::voAnalysisFactory():d_ptr(new voAnalysisFactoryPrivate)
{
  this->registerAnalysis<voANOVAStatistics>("ANOVA");
  this->registerAnalysis<voFoldChange>("Fold Change");
  this->registerAnalysis<voHierarchicalClustering>("Hierarchical Clustering");
  this->registerAnalysis<voKEGGCompounds>("KEGG Compounds");
  this->registerAnalysis<voKEGGPathway>("KEGG Pathway");
  this->registerAnalysis<voKMeansClustering>("KMeans Clustering");
  this->registerAnalysis<voOneZoom>("OneZoom Visualization");
  this->registerAnalysis<voPCAStatistics>("PCA");
  this->registerAnalysis<voPLSStatistics>("PLS");
  this->registerAnalysis<voTTest>("T-Test");
  this->registerAnalysis<voXCorrel>("Cross Correlation");
  this->registerAnalysis<voGeigerModelFitting>("Geiger Tree Model Fitting");
  this->registerAnalysis<voTreeDropTip>("Tree Drop Tip With Data");
  this->registerAnalysis<voTreeDropTipWithoutData>("Tree Drop Tip");
  this->registerAnalysis<voCompareTrees>("Compare Trees");
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
  else if(d->customAnalysisNameToInfoMap.contains(className))
    {
    voCustomAnalysisInformation *info =
      d->customAnalysisNameToInfoMap.value(className);
    voRemoteCustomAnalysis *customAnalysis = new voRemoteCustomAnalysis(info);
    customAnalysis->setObjectName(className);
    customAnalysis->loadInformation(info);
    return customAnalysis;
    }
  else
    {
    qCritical() << "unknown analysis:" << className;
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

//-----------------------------------------------------------------------------
void voAnalysisFactory::addCustomAnalysis(voCustomAnalysisInformation *info)
{
  Q_D(voAnalysisFactory);
  QString analysisName = info->name();
  d->customAnalysisNameToInfoMap[analysisName] = info;
  d->PrettyNameToNameMap.insert(analysisName, analysisName);
  d->NameToPrettyNameMap.insert(analysisName, analysisName);
}
