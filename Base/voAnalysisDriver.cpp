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
#include <QHash>
#include <QSharedPointer>
#include <QDebug>
#include <QMainWindow>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisDriver.h"
#include "voAnalysisFactory.h"
#include "voApplication.h"
#include "voDataModelItem.h"
#include "voDataObject.h"

// --------------------------------------------------------------------------
class voAnalysisDriverPrivate
{
public:
  voAnalysisDriverPrivate();
  virtual ~voAnalysisDriverPrivate();
};

// --------------------------------------------------------------------------
// voAnalysisDriverPrivate methods

// --------------------------------------------------------------------------
voAnalysisDriverPrivate::voAnalysisDriverPrivate()
{
}

// --------------------------------------------------------------------------
voAnalysisDriverPrivate::~voAnalysisDriverPrivate()
{
}

// --------------------------------------------------------------------------
// voAnalysisDriver methods

// --------------------------------------------------------------------------
voAnalysisDriver::voAnalysisDriver(QObject* newParent):
    Superclass(newParent), d_ptr(new voAnalysisDriverPrivate)
{
  analysisNameToInputTypes.insert(
    "ANOVA", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "Cross Correlation", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "Fold Change", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "GeigerTreeModelFitting", QStringList() << "vtkTree" << "vtkTable");
  analysisNameToInputTypes.insert(
    "Hierarchical Clustering", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "KEGG Compounds", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "KEGG Pathway", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "KMeans Clustering", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "OneZoom Visualization", QStringList() << "vtkTree");
  analysisNameToInputTypes.insert(
    "PCA", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "PLS", QStringList() << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "T-Test", QStringList() << "vtkExtendedTable");
}

// --------------------------------------------------------------------------
voAnalysisDriver::~voAnalysisDriver()
{
}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysisForAllInputs(const QString& analysisName, bool acceptDefaultParameter)
{
  if (analysisName.isEmpty())
    {
    qWarning() << "Failed to runAnalysisForAllInputs - AnalysisName is empty";
    return;
    }

  voDataModel * model = voApplication::application()->dataModel();

  // Collect inputs
  QList<voDataModelItem*> targetInputs = model->selectedInputObjects();

  foreach(voDataModelItem* targetInput, targetInputs)
    {
    this->runAnalysis(analysisName, targetInput, acceptDefaultParameter);
    }
}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysis(const QString& analysisName, voDataModelItem* inputTarget, bool acceptDefaultParameter)
{
  if (analysisName.isEmpty())
    {
    qWarning() << "Failed to runAnalysis - AnalysisName is an empty string";
    return;
    }
  if (!inputTarget)
    {
    qWarning() << "Failed to runAnalysis - InputTarget is NULL";
    return;
    }
  voAnalysisFactory * analysisFactory = voApplication::application()->analysisFactory();
  voAnalysis * analysis = analysisFactory->createAnalysis(
        analysisFactory->analysisNameFromPrettyName(analysisName));
  Q_ASSERT(analysis);
  analysis->setParent(qApp);
  analysis->setAcceptDefaultParameterValues(acceptDefaultParameter);
  this->runAnalysis(analysis, inputTarget);
}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysis(voAnalysis * analysis, voDataModelItem* inputTarget)
{
  if (!analysis)
    {
    qWarning() << "Failed to runAnalysis - Analysis is NULL";
    return;
    }
  if (!inputTarget)
    {
    qWarning() << "Failed to runAnalysis - InputTarget is NULL";
    return;
    }

  QScopedPointer<voAnalysis> analysisScopedPtr(analysis);

  // Reset abort execution flag
  analysis->setAbortExecution(false);

  // Clear inputs and outputs
  analysis->removeAllInputs();
  analysis->removeAllOutputs();

  // Initialize input and output
  analysis->initializeOutputInformation();

  //qDebug() << "  " << analysis->numberOfInput() << " inputs expected";

  QString analysisName = analysis->objectName();
  QStringList expectedInputTypes = analysisNameToInputTypes.value(analysisName);

  voDataObject * dataObject = NULL;
  voDataModelItem * childItemForSingleInput = NULL;

  if ( expectedInputTypes.size() == 1  && inputTarget->childItems().size() > 0)
    {
    // For most analyses, only one input is required.
    // Nonetheless, for the convenience of our users, we'll handle the case
    // where they passed in a group of inputs.
    bool inputMatches = false;
    for (int i = 0; i < inputTarget->childItems().size(); ++i)
      {
      childItemForSingleInput =
        dynamic_cast<voDataModelItem*>(inputTarget->child(i));

      if (this->doesInputMatchAnalysis(analysisName, childItemForSingleInput,
                                       true))
        {
        dataObject = childItemForSingleInput->dataObject();
        inputMatches = true;
        break;
        }
      childItemForSingleInput = NULL;
      }
    if (!inputMatches)
      {
      return;
      }
    }
  else
    {
    if (!this->doesInputMatchAnalysis(analysisName, inputTarget, true))
      {
      return;
      }
    dataObject = inputTarget->dataObject();
    }

  // At this point we've verified that our input is good.  Now we need
  // to pass this input into the analysis.
  if (expectedInputTypes.size() == 1)
    {
    // Single input case.
    analysis->addInput(dataObject);
    }
  else
    {
    // Multiple input case.
    for (int i = 0; i < expectedInputTypes.size(); i++)
      {
      voDataModelItem * childItem =
        dynamic_cast<voDataModelItem*>(inputTarget->child(i));
      dataObject = childItem->dataObject();
      analysis->addInput(dataObject);
      }
    }

  // Initialize parameter
  analysis->initializeParameterInformation();

  emit this->aboutToRunAnalysis(analysis);
  if (analysis->abortExecution())
    {
    return;
    }

  bool ret = analysis->run();
  if (!ret)
    {
    qCritical() << "Analysis failed to run " << analysis->objectName();
    return;
    }

  if (childItemForSingleInput == NULL)
    {
    voAnalysisDriver::addAnalysisToObjectModel(
      analysisScopedPtr.take(), inputTarget);
    }
  else
    {
    voAnalysisDriver::addAnalysisToObjectModel(
      analysisScopedPtr.take(), childItemForSingleInput);
    }

  connect(analysis, SIGNAL(outputSet(const QString&, voDataObject*, voAnalysis*)),
    SLOT(onAnalysisOutputSet(const QString&,voDataObject*,voAnalysis*)));

  emit this->analysisAddedToObjectModel(analysis);

  qDebug() << " => Analysis" << analysis->objectName() << " DONE";
}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysisForCurrentInput(
  const QString& analysisName, const QHash<QString, QVariant>& parameters)
{
  qDebug() << "runAnalysisForCurrentInput" << analysisName;

  voDataModel * model = voApplication::application()->dataModel();
  voDataModelItem * inputTarget = model->inputTargetForAnalysis(model->activeAnalysis());
  Q_ASSERT(inputTarget);

  voAnalysisFactory * analysisFactory = voApplication::application()->analysisFactory();
  voAnalysis * newAnalysis = analysisFactory->createAnalysis(
    analysisFactory->analysisNameFromPrettyName(analysisName));
  Q_ASSERT(newAnalysis);
  newAnalysis->initializeParameterInformation(parameters);
  newAnalysis->setAcceptDefaultParameterValues(true);
  this->runAnalysis(newAnalysis, inputTarget);
}

// --------------------------------------------------------------------------
void voAnalysisDriver::updateAnalysis(
  voAnalysis * analysis, const QHash<QString, QVariant>& parameters)
{
  if (!analysis || parameters.count() == 0)
    {
    return;
    }
  // qDebug() << "voAnalysisDriver::updateAnalysis";

  // Update analysis parameter
  analysis->setParameterValues(parameters);
  analysis->setAcceptDefaultParameterValues(true);

  // Clear outputs
  analysis->removeAllOutputs();
  analysis->initializeOutputInformation();

  emit this->aboutToRunAnalysis(analysis);

  bool ret = analysis->run();
  if (!ret)
    {
    qCritical() << "Analysis failed to run " << analysis->objectName();
    return;
    }
}

// --------------------------------------------------------------------------
void voAnalysisDriver::onAnalysisOutputSet(
  const QString& outputName, voDataObject* dataObject, voAnalysis* analysis)
{
  if (outputName.isEmpty() || !dataObject || !analysis)
    {
    return;
    }

  voDataModel * model = voApplication::application()->dataModel();

  voDataModelItem * analysisContainer = model->itemForAnalysis(analysis);
  Q_ASSERT(analysisContainer);
  QList<voDataModelItem*> items =
      model->findItemsWithRole(voDataModelItem::OutputNameRole, outputName, analysisContainer);
  foreach(voDataModelItem* item, items)
    {
    item->setDataObject(dataObject);
    }
}

// --------------------------------------------------------------------------
void voAnalysisDriver::addAnalysisToObjectModel(voAnalysis * analysis,
                                                voDataModelItem* insertLocation)
{
  if (!analysis || !insertLocation)
    {
    return;
    }

  voDataModel * model = voApplication::application()->dataModel();

  // Analysis container
  voDataModelItem * analysisContainer =
      model->addContainer(model->getNextName(analysis->objectName()), insertLocation);
  analysisContainer->setData(QVariant(analysis->uuid()), voDataModelItem::UuidRole);
  analysisContainer->setData(QVariant(true), voDataModelItem::IsAnalysisContainerRole);
  analysisContainer->setData(QVariant(QMetaType::VoidStar, &analysis), voDataModelItem::AnalysisVoidStarRole);

  // Outputs container
  voDataModelItem * outputsContainer = analysisContainer;
//      model->addContainer(QObject::tr("outputs"), analysisContainer);

  // Views container
  voDataModelItem * viewContainer = analysisContainer;
//      model->addContainer(QObject::tr("views"), analysisContainer);

  // Loop over outputs
  foreach(const QString& outputName, analysis->outputNames())
    {
    voDataObject * dataObject = analysis->output(outputName);
    if (!dataObject || dataObject->data().isNull())
      {
      qCritical() << QObject::tr("Analysis ran, but output [%1] is missing.").arg(outputName);
      continue;
      }

    // Append output only if it is associated with a non-empty rawViewType
    QString rawViewType = analysis->rawViewTypeForOutput(outputName);
    if (!rawViewType.isEmpty())
      {
      QString rawViewPrettyName = analysis->rawViewPrettyName(outputName, rawViewType);
      voDataModelItem * outputItem =
          model->addOutput(dataObject, outputsContainer, rawViewType, rawViewPrettyName);
      outputItem->setData(outputName, voDataModelItem::OutputNameRole);
      }

    foreach(const QString& viewType, analysis->viewTypesForOutput(outputName))
      {
      if (viewType.isEmpty()) { continue; }
      QString viewPrettyName = analysis->viewPrettyName(outputName, viewType);
      voDataModelItem * viewItem =
          model->addView(viewType, viewPrettyName, dataObject, viewContainer);
      viewItem->setData(outputName, voDataModelItem::OutputNameRole);
      }
    }
}

// --------------------------------------------------------------------------
bool voAnalysisDriver::doesInputMatchAnalysis(const QString& analysisName,
                                              voDataModelItem* inputTarget,
                                              bool warnOnFail)
{
  if (analysisName.isEmpty())
    {
    qWarning() << "AnalysisName is empty";
    return false;
    }

  QStringList expectedInputTypes = analysisNameToInputTypes.value(analysisName);
  QString expectedInputType = expectedInputTypes.at(0);
  QString providedInputType = inputTarget->dataObject()->type();

  // This analysis requires a single input.
  if ( expectedInputTypes.size() == 1 )
    {
    // Does the type of the provided input match the expected one ?
    if (expectedInputType == providedInputType)
      {
      return true;
      }
    if (warnOnFail)
      {
      qWarning() << QObject::tr("Provided input type %1 is "
        "different from the expected input type %2").
        arg(providedInputType).arg(expectedInputType);
      }
    return false;
    }

  // This analysis requires multiple inputs.
  if (expectedInputTypes.size() != inputTarget->childItems().size())
    {
    if (warnOnFail)
      {
      qWarning() << QObject::tr("Provided input number %1 is "
        "different from the expected input number %2").
        arg(inputTarget->childItems().size()).arg(expectedInputTypes.size());
      }
    return false;
    }
  for (int i = 0; i < expectedInputTypes.size(); i++)
    {
    expectedInputType = expectedInputTypes.at(i);
    voDataModelItem * childItem =  dynamic_cast<voDataModelItem*>(inputTarget->child(i));
    providedInputType = childItem->dataObject()->type();
    if (expectedInputType != providedInputType)
      {
      if (warnOnFail)
        {
        qWarning() << QObject::tr("Provided input type %1 is "
          "different from the expected input type %2").
          arg(providedInputType).arg(expectedInputType);
        }
      return false;
      }
    }
    return true;
}
