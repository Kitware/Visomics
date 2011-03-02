
// Qt includes
#include <QHash>
#include <QExplicitlySharedDataPointer>
#include <QUuid>
#include <QDebug>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>
#include <QtVariantProperty>

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

  bool AcceptDefaultParameterValues;

  QtVariantPropertyManager*          VariantManager;
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
  this->AcceptDefaultParameterValues = false;
  this->VariantManager = new QtVariantPropertyManager(q);
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

namespace
{
// --------------------------------------------------------------------------
void checkForSpaces(const char* className, const char* funcName,
                    const char* varName, const QString& value)
{
  // Check for spaces
  QString msg = QObject::tr("voAnalysis::%1 [%2]- Parameter %3 shouldn't contain any spaces !");
  if (value.contains(" ")){ qCritical() << msg.arg(funcName, className, varName); return; }
}

// --------------------------------------------------------------------------
void checkForLeadingOrTrailingSpaces(const char* className, const char* funcName,
                                     const char* varName, const QString& value)
{
  // Check for heading and trailing spaces
  QString msg =
      QObject::tr("voAnalysis::%1 [%2] - Parameter %2 shouldn't contain heading or trailing spaces !");
  if (value.trimmed() != value){ qCritical() << msg.arg(funcName, className, varName); return; }
}
}

// --------------------------------------------------------------------------
void voAnalysis::addInputType(const QString& inputName, const QString& inputType)
{
  Q_D(voAnalysis);

  const char* className = this->metaObject()->className();
  checkForSpaces(className, "addInputType", "inputName", inputName);
  checkForSpaces(className, "addInputType", "inputType", inputType);

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

  const char* className = this->metaObject()->className();

  checkForLeadingOrTrailingSpaces(className, "addOutputType", "viewPrettyName", viewPrettyName);
  checkForLeadingOrTrailingSpaces(className, "addOutputType", "rawViewPrettyName", rawViewPrettyName);

  checkForSpaces(className, "addOutputType", "outputName", outputName);
  checkForSpaces(className, "addOutputType", "outputType", outputType);
  checkForSpaces(className, "addOutputType", "viewType", viewType);
  checkForSpaces(className, "addOutputType", "rawViewType", rawViewType);

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
  Q_ASSERT(outputName == dataObject->name());
  d->OutputDataObjects.insert(outputName, QExplicitlySharedDataPointer<voDataObject>(dataObject));

  emit this->outputSet(outputName, dataObject, this);
}

// --------------------------------------------------------------------------
voDataObject * voAnalysis::output(const QString& outputName) const
{
  Q_D(const voAnalysis);
  if (!this->hasOutput(outputName))
    {
    return 0;
    }
  //qDebug() << "voAnalysis::output " << outputName;
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
void voAnalysis::initializeParameterInformation(const QHash<QString, QVariant>& parameters)
{
  Q_D(voAnalysis);
  if (d->ParameterInformationInitialized)
    {
    return;
    }
  this->setParameterInformation();
  this->setParameterValues(parameters);

  d->ParameterInformationInitialized = true;
}

// --------------------------------------------------------------------------
void voAnalysis::setParameterValues(const QHash<QString, QVariant>& parameters)
{
  Q_D(voAnalysis);
  foreach(const QString& id, parameters.keys())
    {
    QtVariantProperty* variantProp =
        dynamic_cast<QtVariantProperty*>(d->VariantManager->qtProperty(id));
    Q_ASSERT(variantProp);
    variantProp->setValue(parameters.value(id));
    }
}

// --------------------------------------------------------------------------
QSet<QtVariantProperty*> voAnalysis::topLevelParameterGroups()const
{
  Q_D(const voAnalysis);
  QSet<QtVariantProperty*> topLevelProperties;
  foreach(QtProperty* prop, d->VariantManager->properties())
    {
    if (!prop->isSubProperty())
      {
      QtVariantProperty * variantProp = dynamic_cast<QtVariantProperty*>(prop);
      Q_ASSERT(variantProp);
      topLevelProperties << variantProp;
      }
    }

  return topLevelProperties;
}

// --------------------------------------------------------------------------
int voAnalysis::parameterCount()const
{
  Q_D(const voAnalysis);
  return d->VariantManager->properties().count();
}

// --------------------------------------------------------------------------
void voAnalysis::setAcceptDefaultParameterValues(bool value)
{
  Q_D(voAnalysis);
  d->AcceptDefaultParameterValues = value;
}

// --------------------------------------------------------------------------
bool voAnalysis::acceptDefaultParameterValues()const
{
  Q_D(const voAnalysis);
  return d->AcceptDefaultParameterValues;
}

// --------------------------------------------------------------------------
QtVariantPropertyManager * voAnalysis::propertyManager()const
{
  Q_D(const voAnalysis);
  return d->VariantManager;
}

// --------------------------------------------------------------------------
void voAnalysis::addParameterGroup(const QString& label, const QList<QtProperty*> parameters)
{
  Q_D(voAnalysis);

  QtVariantProperty * group = d->VariantManager->addProperty(
        QtVariantPropertyManager::groupTypeId(), label);

  foreach(QtProperty * param, parameters)
    {
    group->addSubProperty(param);
    }
}

// --------------------------------------------------------------------------
QtVariantProperty* voAnalysis::parameter(const QString& id)const
{
  Q_D(const voAnalysis);
  return dynamic_cast<QtVariantProperty*>(d->VariantManager->qtProperty(id));
}

// --------------------------------------------------------------------------
QString voAnalysis::enumParameter(const QString& id)const
{
  QtVariantProperty * prop = this->parameter(id);
  Q_ASSERT(prop);

  QStringList choices = prop->attributeValue(QLatin1String("enumNames")).toStringList();
  Q_ASSERT(choices.count() > 0);

  return choices.at(prop->value().toInt());
}

// --------------------------------------------------------------------------
QtVariantProperty* voAnalysis::addEnumParameter(const QString& id, const QString& label,
                                                const QStringList& choices, const QString& value)
{
  Q_ASSERT(!label.isEmpty());
  Q_ASSERT(!label.isEmpty());
  Q_ASSERT(!choices.isEmpty());
  Q_ASSERT(value.isEmpty() || choices.contains(value));

  Q_D(voAnalysis);

  QtVariantProperty *param = d->VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), label);

  param->setPropertyId(id);
  param->setAttribute(QLatin1String("enumNames"), choices);
  param->setValue(choices.indexOf(value));

  return param;
}

// --------------------------------------------------------------------------
int voAnalysis::integerParameter(const QString& id)const
{
  QtVariantProperty * prop = this->parameter(id);
  Q_ASSERT(prop);
  return prop->value().toInt();
}

// --------------------------------------------------------------------------
QtVariantProperty*  voAnalysis::addIntegerParameter(const QString& id, const QString& label,
                                                    int minimum, int maximum, int value)
{
  Q_ASSERT(!label.isEmpty());
  Q_ASSERT(!label.isEmpty());

  Q_D(voAnalysis);

  QtVariantProperty * param = d->VariantManager->addProperty(QVariant::Int, label);

  param->setPropertyId(id);
  param->setValue(value);
  param->setAttribute(QLatin1String("minimum"), minimum);
  param->setAttribute(QLatin1String("maximum"), maximum);
  param->setAttribute(QLatin1String("singleStep"), 1);

  return param;
}

// --------------------------------------------------------------------------
double voAnalysis::doubleParameter(const QString& id)const
{
  QtVariantProperty * prop = this->parameter(id);
  Q_ASSERT(prop);
  return prop->value().toDouble();
}

// --------------------------------------------------------------------------
QtVariantProperty*  voAnalysis::addDoubleParameter(const QString& id, const QString& label,
                                                   double minimum, double maximum,
                                                   double value)
{
  Q_ASSERT(!id.isEmpty());
  Q_ASSERT(!label.isEmpty());

  Q_D(voAnalysis);

  QtVariantProperty * param = d->VariantManager->addProperty(QVariant::Double, label);

  param->setPropertyId(id);
  param->setValue(value);
  param->setAttribute(QLatin1String("minimum"), minimum);
  param->setAttribute(QLatin1String("maximum"), maximum);
  param->setAttribute(QLatin1String("singleStep"), 0.01);
  param->setAttribute(QLatin1String("decimals"), 2);

  return param;
}

// --------------------------------------------------------------------------
bool voAnalysis::booleanParameter(const QString& id)const
{
  QtVariantProperty * prop = this->parameter(id);
  Q_ASSERT(prop);
  return prop->value().toBool();
}

// --------------------------------------------------------------------------
QtVariantProperty*  voAnalysis::addBooleanParameter(const QString& id, const QString& label, bool value)
{
  Q_ASSERT(!id.isEmpty());
  Q_ASSERT(!label.isEmpty());

  Q_D(voAnalysis);

  QtVariantProperty * param = d->VariantManager->addProperty(QVariant::Bool, label);
  param->setPropertyId(id);
  param->setValue(value);

  return param;
}
