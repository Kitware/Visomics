
// Qt includes
#include <QHash>
#include <QSharedPointer>
#include <QDebug>

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

  QHash<QString, QSharedPointer<voAnalysis> > Analysis;

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
  Q_D(voAnalysisDriver);
}

// --------------------------------------------------------------------------
voAnalysisDriver::~voAnalysisDriver()
{

}

// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysisForAllInputs(const QString& analysisName)
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
    this->runAnalysis(analysisName, targetInput);
    }

}


// --------------------------------------------------------------------------
void voAnalysisDriver::runAnalysis(const QString& analysisName, voDataModelItem* inputTarget)
{
  Q_D(voAnalysisDriver);
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

  voAnalysis * analysis = 0;
  QStringList keys = d->Analysis.keys();
  if (!d->Analysis.keys().contains(analysisName))
    {
    if (!analysisFactory->registeredAnalysisNames().contains(analysisName))
      {
      qWarning() << "Failed to runAnalysis - Unknown analysis [" << analysisName << "]";
      return;
      }
    analysis = analysisFactory->createAnalysis(analysisName);
    Q_ASSERT(analysis);
    d->Analysis.insert(analysisName, QSharedPointer<voAnalysis>(analysis));
    }
  analysis = d->Analysis.value(analysisName).data();
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

  // Clear inputs and outputs
  analysis->removeAllInputs();
  analysis->removeAllOutputs();

  // Initialize input and output
  analysis->initializeInputInformation();
  analysis->initializeOutputInformation();

  //qDebug() << "  " << analysis->numberOfInput() << " inputs expected";

  int providedInputCount = 1;
  if (analysis->numberOfInput() != providedInputCount)
    {
    qWarning() << "Failed to runAnalysis - Number of input provided"
        << providedInputCount << "is different from the number of input expected"
        << analysis->numberOfInput();
    return;
    }

  QString inputName = analysis->inputNames().value(0);
  voDataObject * dataObject = inputTarget->dataObject();

  // Does the type of the provided input match the expected one ?
  QString expectedInputType = analysis->inputType(inputName);
  QString providedInputType = dataObject->type();
  if (expectedInputType != providedInputType)
    {
    qWarning() << QObject::tr("Failed to runAnalysis - Provided input type %1 is "
                              "different from the expected input type %2").
        arg(providedInputType).arg(expectedInputType);
    return;
    }

  analysis->setInput(inputName, dataObject);

  bool ret = analysis->run();
  if (!ret)
    {
    qCritical() << "Analysis failed to run " << analysis->objectName();
    return;
    }

  voAnalysisDriver::addAnalysisToObjectModel(analysis, inputTarget);

  qDebug() << " => Analysis" << analysis->objectName() << " DONE";
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
  analysisContainer->setData(QVariant(true), voDataModelItem::IsAnalysisContainerRole);

  // Outputs container
  voDataModelItem * outputsContainer =
      model->addContainer(QObject::tr("outputs"), analysisContainer);

  // Views container
  voDataModelItem * viewContainer =
      model->addContainer(QObject::tr("views"), analysisContainer);

  // Loop over outputs
  foreach(const QString& outputName, analysis->outputNames())
    {
    voDataObject * dataObject = analysis->output(outputName);
    if (!dataObject->data())
      {
      qCritical() << QObject::tr("Analysis ran, but output [%1] is missing.").arg(outputName);
      continue;
      }

    // Append output only if it is associated with a non-empty rawViewType
    QString rawViewType = analysis->rawViewTypeForOutput(outputName);
    if (!rawViewType.isEmpty())
      {
      QString rawViewPrettyName = analysis->rawViewPrettyName(outputName, rawViewType);
      model->addOuput(dataObject, outputsContainer, rawViewType, rawViewPrettyName);
      }

    foreach(const QString& viewType, analysis->viewTypesForOutput(outputName))
      {
      if (viewType.isEmpty()) { continue; }
      QString viewPrettyName = analysis->viewPrettyName(outputName, viewType);
      model->addView(viewType, viewPrettyName, dataObject, viewContainer);
      }
    }
}
