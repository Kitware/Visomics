
// Qt includes
#include <QHash>
#include <QExplicitlySharedDataPointer>
#include <QUuid>
#include <QDebug>

// Visomics include
#include "voAnalysis.h"
#include "voDataObject.h"

// --------------------------------------------------------------------------
class voAnalysisPrivate
{
  Q_DECLARE_PUBLIC(voAnalysis);

protected:
  voAnalysis* const q_ptr;

public:
  voAnalysisPrivate(voAnalysis& object);
  ~voAnalysisPrivate();

  void init();

  // TODO Use vtkInformationVector ?
  //vtkInformationVector* InputInformation;
  //vtkInformationVector* OutputInformation;

  QString Uuid;

  QHash<QString, QString> InputInformation;
  QHash<QString, QExplicitlySharedDataPointer<voDataObject> > InputDataObjects;
  QHash<QString, QString> OutputInformation;
  QHash<QString, QString> OutputViewInformation;
  QHash<QString, QString> OutputRawView;
  QHash<QString, QString> OutputViewPrettyName;
  QHash<QString, QString> OutputRawViewPrettyName;
  QHash<QString, QExplicitlySharedDataPointer<voDataObject> > OutputDataObjects;

  bool InputInformationInitialized;
  bool OutputInformationInitialized;
  bool ParameterInformationInitialized;
};

// --------------------------------------------------------------------------
// voAnalysisPrivate methods

// --------------------------------------------------------------------------
voAnalysisPrivate::voAnalysisPrivate(voAnalysis& object) : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void voAnalysisPrivate::init()
{
  Q_Q(voAnalysis);
  this->Uuid = QUuid::createUuid().toString();
  this->InputInformationInitialized = false;
  this->OutputInformationInitialized = false;
  this->ParameterInformationInitialized = false;
}

// --------------------------------------------------------------------------
voAnalysisPrivate::~voAnalysisPrivate()
{
}

// --------------------------------------------------------------------------
// voAnalysis methods

// --------------------------------------------------------------------------
voAnalysis::voAnalysis():d_ptr(new voAnalysisPrivate(*this))
{
  Q_D(voAnalysis);
  d->init();
}

// --------------------------------------------------------------------------
voAnalysis::~voAnalysis()
{
}

// --------------------------------------------------------------------------
QString voAnalysis::uuid()const
{
  Q_D(const voAnalysis);
  return d->Uuid;
}

// --------------------------------------------------------------------------
void voAnalysis::addInputType(const QString& inputName, const QString& inputType)
{
  Q_D(voAnalysis);

  if (this->hasInput(inputName))
    {
    return;
    }
  d->InputInformation.insert(inputName, inputType);
}

// --------------------------------------------------------------------------
QString voAnalysis::inputType(const QString& inputName) const
{
  Q_D(const voAnalysis);
  if (!this->hasInput(inputName))
    {
    return QString();
    }
  return d->InputInformation.value(inputName);
}

// --------------------------------------------------------------------------
int voAnalysis::numberOfInput()
{
  Q_D(const voAnalysis);
  this->initializeInputInformation();
  return d->InputInformation.count();
}

// --------------------------------------------------------------------------
QStringList voAnalysis::inputNames()const
{
  Q_D(const voAnalysis);
  return d->InputInformation.keys();
}

// --------------------------------------------------------------------------
bool voAnalysis::hasInput(const QString& inputName)const
{
  Q_D(const voAnalysis);
  return d->InputInformation.contains(inputName);
}

// --------------------------------------------------------------------------
void voAnalysis::setInput(const QString& inputName, voDataObject * dataObject)
{
  Q_D(voAnalysis);
  if (!this->hasInput(inputName))
    {
    return;
    }
  d->InputDataObjects.insert(inputName, QExplicitlySharedDataPointer<voDataObject>(dataObject));
}

// --------------------------------------------------------------------------
voDataObject * voAnalysis::input(const QString& inputName) const
{
  Q_D(const voAnalysis);
  if (!this->hasInput(inputName))
    {
    return 0;
    }
  return d->InputDataObjects.value(inputName).data();
}

// --------------------------------------------------------------------------
void voAnalysis::removeAllInputs()
{
  Q_D(voAnalysis);
  d->InputDataObjects.clear();
  d->InputInformation.clear();
  d->InputInformationInitialized = false;
}

// --------------------------------------------------------------------------
void voAnalysis::addOutputType(const QString& outputName, const QString& outputType,
                               const QString& viewType, const QString& viewPrettyName,
                               const QString& rawViewType, const QString& rawViewPrettyName)
{
  Q_D(voAnalysis);
  if (this->hasOutput(outputName))
    {
    return;
    }
  d->OutputInformation.insert(outputName, outputType);

  d->OutputViewInformation.insertMulti(outputName, viewType);

  if (!rawViewType.isEmpty())
    {
    d->OutputRawView.insert(outputName, rawViewType);

    if (!rawViewPrettyName.isEmpty())
      {
      d->OutputRawViewPrettyName.insert(outputName + rawViewType, rawViewPrettyName);
      }
    }

  if (!viewPrettyName.isEmpty())
    {
    d->OutputViewPrettyName.insert(outputName + viewType, viewPrettyName);
    }
}

// --------------------------------------------------------------------------
QString voAnalysis::outputType(const QString& outputName) const
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return QString();
    }
  return d->OutputInformation.value(outputName);
}

// --------------------------------------------------------------------------
QString voAnalysis::viewPrettyName(const QString& outputName, const QString& viewType)
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return QString();
    }
  return d->OutputViewPrettyName.value(outputName + viewType, /*defaultValue=*/QString());;
}

// --------------------------------------------------------------------------
int voAnalysis::numberOfOutput()
{
  Q_D(const voAnalysis);
  this->initializeOutputInformation();
  return d->OutputInformation.count();
}

// --------------------------------------------------------------------------
QStringList voAnalysis::outputNames()const
{
  Q_D(const voAnalysis);
  return d->OutputInformation.keys();
}

// --------------------------------------------------------------------------
bool voAnalysis::hasOutput(const QString& outputName)const
{
  Q_D(const voAnalysis);
  return d->OutputInformation.contains(outputName);
}

// --------------------------------------------------------------------------
void voAnalysis::setOutput(const QString& outputName, voDataObject * dataObject)
{
  Q_D(voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return;
    }
  d->OutputDataObjects.insert(outputName, QExplicitlySharedDataPointer<voDataObject>(dataObject));
}

// --------------------------------------------------------------------------
voDataObject * voAnalysis::output(const QString& outputName) const
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return 0;
    }
  return d->OutputDataObjects.value(outputName).data();
}

// --------------------------------------------------------------------------
bool voAnalysis::hasOutputWithViewType(const QString& outputName, const QString& viewType) const
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return false;
    }
  if (d->OutputViewInformation.values().contains(viewType))
    {
    return true;
    }
  return false;
}

// --------------------------------------------------------------------------
QStringList voAnalysis::viewTypesForOutput(const QString& outputName)const
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return QStringList();
    }

  return d->OutputViewInformation.values(outputName);
}

//// --------------------------------------------------------------------------
//QStringList voAnalysis::viewTypes()const
//{
//  Q_D(const voAnalysis);
//  return d->OutputViewInformation.values();
//}

// --------------------------------------------------------------------------
bool voAnalysis::hasOutputWithRawViewType(const QString& outputName, const QString& rawViewType) const
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return false;
    }
  if (d->OutputRawView.values().contains(rawViewType))
    {
    return true;
    }
  return false;
}

// --------------------------------------------------------------------------
QString voAnalysis::rawViewTypeForOutput(const QString& outputName)const
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return QString();
    }

  return d->OutputRawView.value(outputName, /*defaultValue=*/ QString());
}

// --------------------------------------------------------------------------
QString voAnalysis::rawViewPrettyName(const QString& outputName, const QString& rawViewType)
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return QString();
    }

  return d->OutputRawViewPrettyName.value(outputName + rawViewType, /*defaultValue=*/QString());;
}

// --------------------------------------------------------------------------
void voAnalysis::removeAllOutputs()
{
  Q_D(voAnalysis);
  d->OutputDataObjects.clear();
  d->OutputInformation.clear();
  d->OutputViewInformation.clear();
  d->OutputRawView.clear();
  d->OutputViewPrettyName.clear();
  d->OutputRawViewPrettyName.clear();
  d->OutputInformationInitialized = false;
}

// --------------------------------------------------------------------------
bool voAnalysis::run()
{
  return this->execute();
}

// --------------------------------------------------------------------------
bool voAnalysis::execute()
{
  return true;
}

// --------------------------------------------------------------------------
void voAnalysis::initializeInputInformation()
{
  Q_D(voAnalysis);
  if (d->InputInformationInitialized)
    {
    return;
    }
  this->setInputInformation();
  d->InputInformationInitialized = true;
}

// --------------------------------------------------------------------------
void voAnalysis::initializeOutputInformation()
{
  Q_D(voAnalysis);
  if (d->OutputInformationInitialized)
    {
    return;
    }
  this->setOutputInformation();
  d->OutputInformationInitialized = true;
}

// --------------------------------------------------------------------------
void voAnalysis::initializeParameterInformation()
{
  Q_D(voAnalysis);
  if (d->ParameterInformationInitialized)
    {
    return;
    }
  this->setParameterInformation();
  d->ParameterInformationInitialized = true;
}

