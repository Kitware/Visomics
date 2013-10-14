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
#include <QFile>
#include <QHash>
#include <QSharedPointer>
#include <QDebug>
#include <QMainWindow>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QtGui/QMessageBox>
#include <QtCore/QUrl>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisDriver.h"
#include "voAnalysisFactory.h"
#include "voApplication.h"
#include "voCustomAnalysisInformation.h"
#include "voDataModelItem.h"
#include "voDataObject.h"
#include "voViewManager.h"
#include "voRemoteCustomAnalysis.h"


// --------------------------------------------------------------------------
class voAnalysisDriverPrivate
{
public:
  voAnalysisDriverPrivate();
  virtual ~voAnalysisDriverPrivate();

  QUrl remoteAnalysisUrl;
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
    "Geiger Tree Model Fitting", QStringList() << "vtkTree" << "vtkExtendedTable");
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
  analysisNameToInputTypes.insert(
    "Tree Drop Tip", QStringList() << "vtkTree");
  analysisNameToInputTypes.insert(
    "Tree Drop Tip With Data", QStringList() << "vtkTree" << "vtkExtendedTable");
  analysisNameToInputTypes.insert(
    "Compare Trees", QStringList() << "vtkTree");
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
voAnalysis * voAnalysisDriver::createAnalysis(const QString& analysisName)
{
  if (analysisName.isEmpty())
    {
    qWarning() << "Failed to createAnalysis - AnalysisName is an empty string";
    return NULL;
    }
  voAnalysisFactory * analysisFactory = voApplication::application()->analysisFactory();
  voAnalysis * analysis = analysisFactory->createAnalysis(
        analysisFactory->analysisNameFromPrettyName(analysisName));
  Q_ASSERT(analysis);
  analysis->setParent(qApp);
  return analysis;
}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysis(const QString& analysisName, voDataModelItem* inputTarget, bool acceptDefaultParameter)
{
  if (!inputTarget)
    {
    qWarning() << "Failed to runAnalysis - InputTarget is NULL";
    return;
    }
  voAnalysis * analysis = this->createAnalysis(analysisName);
  voView * currentView = voApplication::application()->viewManager()->getView(inputTarget->uuid());
  analysis->setView(currentView);
  if (!analysis)
    {
    return;
    }
  analysis->setAcceptDefaultParameterValues(acceptDefaultParameter);
  this->runAnalysis(analysis, inputTarget);
}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysis(const QString& analysisName,
                                   voDataModelItem* inputTarget,
                                   const QHash<QString, QVariant>& parameters)
{
  if (!inputTarget)
    {
    qWarning() << "Failed to runAnalysis - InputTarget is NULL";
    return;
    }
  voAnalysis * analysis = this->createAnalysis(analysisName);
  if (!analysis)
    {
    return;
    }

  analysis->initializeParameterInformation();
  analysis->updateDynamicParameters();
  analysis->setParameterValues(parameters);
  analysis->setAcceptDefaultParameterValues(true);
  this->runAnalysis(analysis, inputTarget);
}

voAnalysisTask::voAnalysisTask(voAnalysis *analysis, voDataModelItem* insertLocation)
  : m_analysis(analysis), m_insertLocation(insertLocation)
{
  connect(m_analysis, SIGNAL(complete()), this, SIGNAL(complete()));
  connect(m_analysis, SIGNAL(error(const QString&)), this, SIGNAL(error(const QString&)));
}

bool voAnalysisTask::run()
{
  return m_analysis->run();
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

  // Reset abort execution flag
  analysis->setAbortExecution(false);

  // Clear inputs and outputs
  analysis->removeAllInputs();
  analysis->removeAllOutputs();

  // Initialize input and output
  analysis->initializeOutputInformation();

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
  analysis->updateDynamicParameters();

  emit this->aboutToRunAnalysis(analysis);
  if (analysis->abortExecution())
    {
    return;
    }

  voDataModelItem *insertLocation;

  if (childItemForSingleInput == NULL)
    {
    insertLocation  = inputTarget;
    }
  else
    {
    insertLocation = childItemForSingleInput;
    }

  voAnalysisTask *task  = new voAnalysisTask(analysis, insertLocation);

  connect(task, SIGNAL(complete()), 
      this, SLOT(analysisComplete()));
  connect(task, SIGNAL(error(const QString&)),
      this, SLOT(analysisError(const QString&)));

  // If we are dealing with remote analysis we need to connect up the extra
  // signals.
  voRemoteCustomAnalysis *remoteAnalysis
    = qobject_cast<voRemoteCustomAnalysis *>(analysis);
  if (remoteAnalysis) 
    {
    connect(remoteAnalysis, SIGNAL(urlRequired(QUrl *)),
            this, SLOT(provideRemoteAnalysisUrl(QUrl *)));
    connect(remoteAnalysis, SIGNAL(invalidCredentials()),
            this, SLOT(onInvalidCredentials()));
    connect(remoteAnalysis, SIGNAL(analysisSubmitted()),
            this, SLOT(onAnalysisSubmitted()));
    }

  bool ret = task->run();
}

void voAnalysisDriver::analysisComplete()
{
  voAnalysisTask *task = qobject_cast<voAnalysisTask*>(sender());
  voAnalysis *analysis = task->analysis();

  voAnalysisDriver::addAnalysisToObjectModel(
      analysis, task->insertLocation());

  connect(analysis, SIGNAL(outputSet(const QString&, voDataObject*, voAnalysis*)),
    SLOT(onAnalysisOutputSet(const QString&,voDataObject*,voAnalysis*)));

  connect(analysis, SIGNAL(ensembleOutputSet(const QString&, QList<voDataObject*>, voAnalysis*)),
    SLOT(onAnalysisEnsembleOutputSet(const QString&, QList<voDataObject*>, voAnalysis*)));
  emit this->analysisAddedToObjectModel(analysis);

  qDebug() << " => Analysis" << analysis->objectName() << " DONE";

  delete task;

  QApplication::restoreOverrideCursor();
}

void voAnalysisDriver::analysisError(const QString &errorString) {
  Q_D(voAnalysisDriver);

  // Clear cached connection detail in case they are the problem.
  d->remoteAnalysisUrl.clear();

  voAnalysisTask *task = qobject_cast<voAnalysisTask*>(sender());

  // If a message was provided display it, otherwise the user may have just
  // cancelled the analysis.
  if (!errorString.isEmpty())
    {
    QMessageBox::critical(voApplication::application()->mainWindow(),
        "Analysis Error", errorString,  QMessageBox::Ok);
    }
  task->analysis()->deleteLater();
  delete task;

  QApplication::restoreOverrideCursor();
}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysisForCurrentInput(
  const QString& analysisName, const QHash<QString, QVariant>& parameters)
{

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
void voAnalysisDriver::onAnalysisEnsembleOutputSet(
  const QString& outputEnsembleName, QList<voDataObject*> dataObjectList, voAnalysis* analysis)
{
  if (outputEnsembleName.isEmpty() || dataObjectList.isEmpty() || !analysis)
    {
    return;
    }

  voDataModel * model = voApplication::application()->dataModel();

  voDataModelItem * analysisContainer = model->itemForAnalysis(analysis);
  Q_ASSERT(analysisContainer);

  QList<voDataModelItem*> items =
      model->findItemsWithRole(voDataModelItem::OutputNameRole, outputEnsembleName, analysisContainer);
  foreach(voDataModelItem* item, items)
    {
    item->setDataObjectList(dataObjectList);
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
voDataModelItem *  voAnalysisDriver::addEnsembleOutputToObjectModel(const QString& outputName, voAnalysis * analysis, voDataModelItem* parent)
{

  voDataObject * dataObject = analysis->ensembleOutput(outputName);
  if (!dataObject)// || dataObject->data().isNull())
    {
    qCritical() << QObject::tr("Analysis ran, but output [%1] is missing.").arg(outputName);
    return NULL;
    }

  voDataModel * model = voApplication::application()->dataModel();
  voDataModelItem * outputItem = NULL;
  // Append output only if it is associated with a non-empty rawViewType
  QString rawViewType = analysis->rawViewTypeForEnsembleOutput(outputName);

  if (!rawViewType.isEmpty())
    {
    QString rawViewPrettyName = analysis->rawViewPrettyName(outputName, rawViewType);
    outputItem =
      model->addOutput(dataObject, parent, rawViewType, rawViewPrettyName);
    outputItem->setData(outputName, voDataModelItem::OutputNameRole);

    }

    outputItem->setType(voDataModelItem::OutputType);

  return outputItem;

}
// --------------------------------------------------------------------------
voDataModelItem *  voAnalysisDriver::addOutputToObjectModel(const QString& outputName, voAnalysis * analysis, voDataModelItem* parent)
{

  voDataObject * dataObject = analysis->output(outputName);
  if (!dataObject)// || dataObject->data().isNull())
    {
    qCritical() << QObject::tr("Analysis ran, but output [%1] is missing.").arg(outputName);
    return NULL;
    }

  voDataModel * model = voApplication::application()->dataModel();
  voDataModelItem * outputItem = NULL;
  // Append output only if it is associated with a non-empty rawViewType
  QString rawViewType = analysis->rawViewTypeForOutput(outputName);
  if (!rawViewType.isEmpty())
    {
    QString rawViewPrettyName = analysis->rawViewPrettyName(outputName, rawViewType);
    outputItem =
      model->addOutput(dataObject, parent, rawViewType, rawViewPrettyName);
    outputItem->setData(outputName, voDataModelItem::OutputNameRole);
    }

  foreach(const QString& viewType, analysis->viewTypesForOutput(outputName))
    {
    if (viewType.isEmpty()) { continue; }
    QString viewPrettyName = analysis->viewPrettyName(outputName, viewType);
    outputItem =
      model->addView(viewType, viewPrettyName, dataObject, parent);
    outputItem->setData(outputName, voDataModelItem::OutputNameRole);
    }

  return outputItem;

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


  QStringList singleOutputNames = analysis->outputNames();
  // process ensemble output first
  foreach (const QString & ensembleOutputName, analysis->ensembleOutputNames())
    {
    voDataModelItem * ensembleOutputItem = addEnsembleOutputToObjectModel(ensembleOutputName, analysis, analysisContainer);

    QStringList childOutputNames = analysis->childNameListOfEnsembleOutput(ensembleOutputName);
    for (int i = 0; i < childOutputNames.size(); i++)
      {
      QString childOutputName = childOutputNames[i];
      // add child outputs under the ensemble output
      voDataModelItem * childItem = addOutputToObjectModel(childOutputName, analysis, ensembleOutputItem);
      singleOutputNames.removeAll(childOutputName);
      ensembleOutputItem->addChildItem(childItem);
      }
    }

  // Loop over non-ensemble outputs
  foreach(const QString& outputName, singleOutputNames)
    {
    addOutputToObjectModel(outputName, analysis, analysisContainer);
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
    // Special case to allow chained analyses
    if (expectedInputType == "vtkExtendedTable" &&
      providedInputType == "vtkTable")
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

// --------------------------------------------------------------------------
int voAnalysisDriver::numberOfInputsForAnalysis(QString analysisName)
{
  if (!analysisNameToInputTypes.contains(analysisName))
    {
    return 0;
    }

  return analysisNameToInputTypes.value(analysisName).size();
}

// --------------------------------------------------------------------------
void voAnalysisDriver::loadAnalysisFromScript(const QString& xmlFileName,
  const QString& rScriptFileName)
{
  QFile file(xmlFileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    qCritical() << "Could not open " << xmlFileName << " for reading!";
    return;
    }

  QFile scriptFile(rScriptFileName);
  if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    qCritical() << "Could not open " << rScriptFileName << " for reading!";
    return;
    }
  QTextStream textStream(&scriptFile);
  QString scriptContents = textStream.readAll();
  scriptFile.close();

  voCustomAnalysisInformation *analysisInformation =
    new voCustomAnalysisInformation(this);

  QXmlStreamReader stream(&file);
  stream.readNextStartElement();  // analysis
  QStringRef analysisName = stream.attributes().value("name");
  analysisInformation->setName(analysisName.toString());
  analysisInformation->setScript(scriptContents);

  // get name & type for each input
  stream.readNextStartElement();  // input
  QString tagName = stream.name().toString();
  if (tagName != "inputs")
    {
    qCritical() << "expected inputs, found " << tagName << "(" << rScriptFileName << ")";
    return;
    }

  stream.readNextStartElement(); //input
  tagName = stream.name().toString();
  while (tagName != "inputs" && !stream.atEnd())
    {
    if (tagName != "input")
      {
      qCritical() << "expected input, found " << tagName;
      return;
      }

    voCustomAnalysisData *inputData =
      new voCustomAnalysisData(analysisInformation);
    QStringRef inputName = stream.attributes().value("name");
    QStringRef inputType = stream.attributes().value("type");
    inputData->setName(inputName.toString());
    inputData->setType(inputType.toString());
    analysisInformation->addInput(inputData);
    stream.readNext();
    stream.readNextStartElement();
    tagName = stream.name().toString();
    if (tagName == "inputs" || tagName == "outputs")
      {
      break;
      }
    }

  // get name & type for each output
  stream.readNext();
  while (!stream.isStartElement())
    {
    stream.readNext(); // outputs
    }
  tagName = stream.name().toString();
  if (tagName != "outputs")
    {
    qCritical() << "expected outputs, found " << tagName;
    }

  stream.readNextStartElement(); //output
  tagName = stream.name().toString();
  while (tagName != "outputs" && !stream.atEnd())
    {
    if (tagName != "output")
      {
      qCritical() << "expected output, found " << tagName;
      return;
      }

    voCustomAnalysisData *outputData =
      new voCustomAnalysisData(analysisInformation);
    QStringRef outputName = stream.attributes().value("name");
    QStringRef outputType = stream.attributes().value("type");
    outputData->setName(outputName.toString());
    outputData->setType(outputType.toString());
    analysisInformation->addOutput(outputData);
    stream.readNext();
    stream.readNextStartElement();
    tagName = stream.name().toString();
    if (tagName == "outputs" || tagName == "parameters")
      {
      break;
      }
    }

  // parse parameter information (if any)
  while (!stream.isStartElement() && !stream.atEnd())
    {
    stream.readNext(); // parameters
    }
  tagName = stream.name().toString();
  if (tagName == "parameters")
    {
    stream.readNextStartElement(); //parameter
    tagName = stream.name().toString();
    while (tagName != "parameters" && !stream.atEnd())
      {
      if (tagName != "parameter")
        {
        qCritical() << "expected parameter, found " << tagName;
        return;
        }

      QStringRef parameterName = stream.attributes().value("name");
      QStringRef parameterType = stream.attributes().value("type");
      voCustomAnalysisParameter *parameter =
        new voCustomAnalysisParameter(analysisInformation);
      parameter->setName(parameterName.toString());
      parameter->setType(parameterType.toString());

      // read parameter tags
      while (stream.readNextStartElement())
        {
        tagName = stream.name().toString();
        if (tagName == "parameter")
          {
          break;
          }
        QString fieldValue = stream.readElementText();
        voCustomAnalysisParameterField *field =
          new voCustomAnalysisParameterField(parameter);
        field->setName(tagName);
        field->setValue(fieldValue);
        parameter->addField(field);
        }
      analysisInformation->addParameter(parameter);
      stream.readNext();
      if (!stream.readNextStartElement())
        {
        break;
        }
      tagName = stream.name().toString();
      }
    }

  // record what type of input(s) this analysis expects
  QStringList inputTypes;
  voCustomAnalysisData *analysisInput;
  foreach(analysisInput, analysisInformation->inputs())
    {
    if (analysisInput->type() == "Table")
      {
      inputTypes << "vtkExtendedTable";
      }
    else if(analysisInput->type() == "Tree")
      {
      inputTypes << "vtkTree";
      }
    else
      {
      qCritical() << "unrecognized input type:" << analysisInput->type();
      return;
      }
    }
  analysisNameToInputTypes.insert(analysisInformation->name(), inputTypes);

  // register this new analysis with our factory.
  voAnalysisFactory * analysisFactory =
    voApplication::application()->analysisFactory();
  analysisFactory->addCustomAnalysis(analysisInformation);
  emit this->addedCustomAnalysis(analysisInformation->name());
}

void voAnalysisDriver::provideRemoteAnalysisUrl(QUrl *url)
{
  Q_D(voAnalysisDriver);

  if (d->remoteAnalysisUrl.isEmpty())
    {
    // Request url from main window
    emit urlRequired(url);

    // Cache the result
    d->remoteAnalysisUrl = *url;
    }
  else
    {
    *url = d->remoteAnalysisUrl;
    }
}

void voAnalysisDriver::updateRemoteAnalysisUrl(QUrl * url)
{
  Q_D(voAnalysisDriver);
  d->remoteAnalysisUrl = *url;
}

void voAnalysisDriver::onInvalidCredentials()
{
  Q_D(voAnalysisDriver);
  // invalidate the cached url
  d->remoteAnalysisUrl.clear();

  // Tell the user
  QMessageBox::critical(voApplication::application()->mainWindow(),
      "Authentication error", "Invalid username or password",  QMessageBox::Ok);
}

void voAnalysisDriver::onAnalysisSubmitted()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}
