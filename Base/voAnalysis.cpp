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
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QExplicitlySharedDataPointer>
#include <QFile>
#include <QHash>
#include <QUuid>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>
#include <QtVariantProperty>

// Visomics include
#include "voApplication.h"
#include "voAnalysis.h"
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voDataObject.h"
#include "voInputFileDataObject.h"
#include "voIOManager.h"
#include "voView.h"

// VTK includes
#include <vtkDataObject.h>
#include <vtkExtendedTable.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>
#include <vtkTree.h>

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

  QString Uuid;
  voView* AnalysisView;

  QList<QExplicitlySharedDataPointer<voDataObject> > InputDataObjects;
  QHash<QString, QString> OutputInformation;
  QHash<QString, QStringList> EnsembleOutputInformation;// <ensembel ouput name, a list of child output names>
  QHash<QString, QString> OutputViewInformation;
  QHash<QString, QString> OutputRawView;
  QHash<QString, QString> OutputViewPrettyName;
  QHash<QString, QString> OutputRawViewPrettyName;
  QHash<QString, QExplicitlySharedDataPointer<voDataObject> > OutputDataObjects;
  QHash<QString, QExplicitlySharedDataPointer<voDataObject> > EnsembleOutputDataObjects;
  QSet<QPair<QString, QString> > DynamicParameters;
  QHash<QString, QString> DynamicParameterValues;

  bool OutputInformationInitialized;
  bool ParameterInformationInitialized;

  bool AcceptDefaultParameterValues;

  bool AbortExecution;

  QString OutputDirectory;
  bool WriteOutputsToFilesEnabled;

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
  this->OutputInformationInitialized = false;
  this->ParameterInformationInitialized = false;
  this->AcceptDefaultParameterValues = false;
  this->AbortExecution = false;
  this->OutputDirectory = QLatin1String(".");
  this->WriteOutputsToFilesEnabled = false;
  this->VariantManager = new QtVariantPropertyManager(q);
  this->AnalysisView = NULL;
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
voView * voAnalysis::getView()const
{
  Q_D(const voAnalysis);
  return d->AnalysisView;
}

// --------------------------------------------------------------------------
void voAnalysis::setView(voView * view)
{
  Q_D(voAnalysis);
  d->AnalysisView = view;
}

// --------------------------------------------------------------------------

namespace
{
// --------------------------------------------------------------------------
bool checkIfEmpty(const char* className, const char* funcName,
                  const char* varName, const QString& value)
{
  // Check for spaces
  QString msg = QObject::tr("voAnalysis::%1 [%2]- Parameter %3 shouldn't be empty !");
  if (value.trimmed().isEmpty()){ qCritical() << msg.arg(funcName, className, varName); return true; }
  return false;
}
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
      QObject::tr("voAnalysis::%1 [%2] - Parameter %2 shouldn't contain leading or trailing spaces !");
  if (value.trimmed() != value){ qCritical() << msg.arg(funcName, className, varName); return; }
}
}

// --------------------------------------------------------------------------
void voAnalysis::addInput(voDataObject * dataObject)
{
  Q_D(voAnalysis);
  if (dataObject == 0)
    {
    return;
    }
  d->InputDataObjects << QExplicitlySharedDataPointer<voDataObject>(dataObject);
}

// --------------------------------------------------------------------------
voDataObject * voAnalysis::input(int i) const
{
  Q_D(const voAnalysis);
  if (i >= d->InputDataObjects.size())
    {
    return 0;
    }
  return d->InputDataObjects.at(i).data();
}

// --------------------------------------------------------------------------
void voAnalysis::removeAllInputs()
{
  Q_D(voAnalysis);
  d->InputDataObjects.clear();
}


// --------------------------------------------------------------------------
void voAnalysis::addEnsembleOutputType(const QString& ensembleOutputName, const QString& outputType,
                               const QString& rawViewType, const QString& rawViewPrettyName,
                               const QStringList childOutputNames)
{
  Q_D(voAnalysis);
  const char* className = this->metaObject()->className();
  checkForLeadingOrTrailingSpaces(className, "addOutputType", "rawViewPrettyName", rawViewPrettyName);

  checkForSpaces(className, "addEnsembleOutputType", "outputName", ensembleOutputName);
  checkForSpaces(className, "addEnsembleOutputType", "outputType", outputType);
  checkForSpaces(className, "addEnsembleOutputType", "rawViewType", rawViewType);

  if (checkIfEmpty(className, "addEnsembleOutputType", "outputName", ensembleOutputName) ||
      checkIfEmpty(className, "addEnsembleOutputType", "outputType", outputType))
    {
    return;
    }

  if (this->hasEnsembleOutput(ensembleOutputName))
    {
    return;
    }

  d->EnsembleOutputInformation.insert(ensembleOutputName, childOutputNames);

  if (!rawViewType.isEmpty())
    {
    d->OutputRawView.insert(ensembleOutputName, rawViewType);

    if (!rawViewPrettyName.isEmpty())
      {
      d->OutputRawViewPrettyName.insert(ensembleOutputName + rawViewType, rawViewPrettyName);
      }
    }

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

  if (checkIfEmpty(className, "addOutputType", "outputName", outputName) ||
      checkIfEmpty(className, "addOutputType", "outputType", outputType))
    {
    return;
    }

  if (this->hasOutput(outputName))
    {
    return;
    }
  d->OutputInformation.insert(outputName, outputType);

  if (!viewType.isEmpty())
    {
    d->OutputViewInformation.insertMulti(outputName, viewType);

    if (!viewPrettyName.isEmpty())
      {
      d->OutputViewPrettyName.insert(outputName + viewType, viewPrettyName);
      }
    }

  if (!rawViewType.isEmpty())
    {
    d->OutputRawView.insert(outputName, rawViewType);

    if (!rawViewPrettyName.isEmpty())
      {
      d->OutputRawViewPrettyName.insert(outputName + rawViewType, rawViewPrettyName);
      }
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
void voAnalysis::setViewPrettyName(const QString& outputName, const QString& viewType, const QString& viewPrettyName)
{
  Q_D(voAnalysis);

  if (!this->hasOutputWithViewType(outputName, viewType))
    {
    return;
    }

  const char* className = this->metaObject()->className();
  checkForLeadingOrTrailingSpaces(className, "setViewPrettyName", "viewPrettyName", viewPrettyName);

  if (viewPrettyName.isEmpty())
    {
    d->OutputViewPrettyName.remove(outputName + viewType);
    }
  else
    {
    d->OutputViewPrettyName.insert(outputName + viewType, viewPrettyName);
    }
}

// --------------------------------------------------------------------------
int voAnalysis::numberOfOutput()
{
  Q_D(const voAnalysis);
  return d->OutputInformation.count();
}

// --------------------------------------------------------------------------
QStringList voAnalysis::outputNames()const
{
  Q_D(const voAnalysis);
  QStringList names = d->OutputInformation.keys();
  names.sort();
  return names;
}

// --------------------------------------------------------------------------
QStringList voAnalysis::ensembleOutputNames()const
{
  Q_D(const voAnalysis);
  QStringList names = d->EnsembleOutputInformation.keys();
  names.sort();
  return names;
}

// --------------------------------------------------------------------------
QStringList voAnalysis::childNameListOfEnsembleOutput(const QString ensembleOutputName)const
{
  Q_D(const voAnalysis);
  QStringList names = d->EnsembleOutputInformation.value(ensembleOutputName);
  return names;
}
// --------------------------------------------------------------------------
bool voAnalysis::hasOutput(const QString& outputName)const
{
  Q_D(const voAnalysis);
  return d->OutputInformation.contains(outputName);
}

// --------------------------------------------------------------------------
bool voAnalysis::hasEnsembleOutput(const QString& outputName)const
{
  Q_D(const voAnalysis);
  return d->EnsembleOutputInformation.contains(outputName);
}

// --------------------------------------------------------------------------
int voAnalysis::getNumberOfEnsembleOutput()
{
  Q_D(const voAnalysis);
  return d->EnsembleOutputInformation.size();
}

// --------------------------------------------------------------------------
void voAnalysis::setEnsembleOutput(const QString& ensembleName, QList<voDataObject*>ensembleDataObjectList)
{//ensemble output item contains a list of child items, each of which contains a dataobject
  Q_D(voAnalysis);

  //create an empty ensemble data Object
  voDataObject * ensembleDataObject = new voDataObject(ensembleName,NULL);

  d->EnsembleOutputDataObjects.insert(ensembleName, QExplicitlySharedDataPointer<voDataObject>(ensembleDataObject));

  emit this->ensembleOutputSet(ensembleName, ensembleDataObjectList, this);
}

// --------------------------------------------------------------------------
void voAnalysis::setOutput(const QString& outputName, voDataObject * dataObject)
{
  Q_D(voAnalysis);
  if (!dataObject ||
      !this->hasOutput(outputName))
    {
    return;
    }
  if (outputName != dataObject->name())
    {
    qCritical() << tr("voAnalysis::setOutput [%1]- outputName [%2] doesn't match dataObject name [%3] !")
                   .arg(this->metaObject()->className()).arg(outputName).arg(dataObject->name());
    return;
    }
  d->OutputDataObjects.insert(outputName, QExplicitlySharedDataPointer<voDataObject>(dataObject));

  emit this->outputSet(outputName, dataObject, this);
}

// --------------------------------------------------------------------------
voDataObject * voAnalysis::ensembleOutput(const QString& outputName) const
{
  Q_D(const voAnalysis);
  if (!this->hasEnsembleOutput(outputName))
    {
    return 0;
    }
  return d->EnsembleOutputDataObjects.value(outputName).data();
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
QString voAnalysis::rawViewTypeForEnsembleOutput(const QString& outputName)const
{
  Q_D(const voAnalysis);
  if (!this->hasEnsembleOutput(outputName))
    {
    return QString();
    }
   // rawView types for ensemble output is stored in outputRawView
  return d->OutputRawView.value(outputName, /*defaultValue=*/ QString());
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
void voAnalysis::setRawViewPrettyName(const QString& outputName, const QString& rawViewType, const QString& rawViewPrettyName)
{
  Q_D(voAnalysis);

  if (!this->hasOutputWithRawViewType(outputName, rawViewType))
    {
    return;
    }

  const char* className = this->metaObject()->className();
  checkForLeadingOrTrailingSpaces(className, "setRawViewPrettyName", "rawViewPrettyName", rawViewPrettyName);

  if (rawViewPrettyName.isEmpty())
    {
    d->OutputRawViewPrettyName.remove(outputName + rawViewType);
    }
  else
    {
    d->OutputRawViewPrettyName.insert(outputName + rawViewType, rawViewPrettyName);
    }
}

// --------------------------------------------------------------------------
void voAnalysis::removeAllOutputs()
{
  Q_D(voAnalysis);
  d->OutputDataObjects.clear();
  d->EnsembleOutputDataObjects.clear();
  d->OutputInformation.clear();
  d->EnsembleOutputInformation.clear();
  d->OutputViewInformation.clear();
  d->OutputRawView.clear();
  d->OutputViewPrettyName.clear();
  d->OutputRawViewPrettyName.clear();
  d->OutputInformationInitialized = false;
}

// --------------------------------------------------------------------------
bool voAnalysis::abortExecution()const
{
  Q_D(const voAnalysis);
  return d->AbortExecution;
}

// --------------------------------------------------------------------------
void voAnalysis::setAbortExecution(bool abortExecutionValue)
{
  Q_D(voAnalysis);
  d->AbortExecution = abortExecutionValue;
}

// --------------------------------------------------------------------------
QString voAnalysis::outputDirectory()const
{
  Q_D(const voAnalysis);
  return d->OutputDirectory;
}

// --------------------------------------------------------------------------
void voAnalysis::setOutputDirectory(const QString& directory)
{
  Q_D(voAnalysis);
  d->OutputDirectory = directory;
}

// --------------------------------------------------------------------------
bool voAnalysis::writeOutputsToFilesEnabled()const
{
  Q_D(const voAnalysis);
  return d->WriteOutputsToFilesEnabled;
}

// --------------------------------------------------------------------------
void voAnalysis::setWriteOutputsToFilesEnabled(bool enabled)
{
  Q_D(voAnalysis);
  d->WriteOutputsToFilesEnabled = enabled;
}

// --------------------------------------------------------------------------
bool voAnalysis::run()
{
  Q_D(voAnalysis);
  bool success = this->execute();
  if (success && d->WriteOutputsToFilesEnabled)
    {
    this->writeOutputsToFiles(d->OutputDirectory);
    }
  return success;
}

// --------------------------------------------------------------------------
bool voAnalysis::execute()
{
  return true;
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
void voAnalysis::writeOutputsToFiles(const QString& directory) const
{
  QString inputHash;
  voInputFileDataObject * inputDataObject = qobject_cast<voInputFileDataObject*>(this->input());
  if (inputDataObject)
    {
    QFile inputFile(inputDataObject->fileName());
    if (inputFile.open(QIODevice::ReadOnly))
      {
      QByteArray sha1 = QCryptographicHash::hash(inputFile.readAll(), QCryptographicHash::Md5);
      inputHash = sha1.toHex();
      inputHash.append("_");
      inputFile.close();
      }
    }

  QDir().mkpath(directory);

  foreach(const QString& outputName, this->outputNames())
    {
    voDataObject * dataObject = this->output(outputName);
    if (!dataObject || !dataObject->isVTKDataObject() || !dataObject->dataAsVTKDataObject())
      {
      continue;
      }
    QString filename("%1/%2%3_%4.vtk"); // <directory>/(<inputHash>_)<analysisName>_<outputName>.vtk
    filename = filename.arg(QDir::cleanPath(directory)).arg(inputHash).arg(this->metaObject()->className()).arg(outputName);
    bool success = voIOManager::writeDataObjectToFile(dataObject->dataAsVTKDataObject(), filename);
    if (!success)
      {
      qCritical() << this->metaObject()->className()
                  << "- Failed to write output" << outputName << "of type" << dataObject->type()
                  << "into file" << filename;
      }
    }
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
QSet<QPair<QString, QString> > voAnalysis::dynamicParameters()const
{
  Q_D(const voAnalysis);
  return d->DynamicParameters;
}

// --------------------------------------------------------------------------
int voAnalysis::parameterCount()const
{
  Q_D(const voAnalysis);
  return d->VariantManager->properties().count() + d->DynamicParameters.count();
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
QString voAnalysis::parameterDescription()const
{
  return QString();
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
QString voAnalysis::dynamicParameter(const QString& label)const
{
  Q_D(const voAnalysis);
  return d->DynamicParameterValues[label];
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
  Q_ASSERT(!id.isEmpty());
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
QtVariantProperty* voAnalysis::updateEnumParameter(const QString& id,
                                                   const QStringList& choices,
                                                   const QString& value)
{
  Q_ASSERT(!id.isEmpty());
  Q_ASSERT(!choices.isEmpty());
  Q_ASSERT(value.isEmpty() || choices.contains(value));

  Q_D(voAnalysis);

  // check if this property has already been added
  QtVariantProperty *param = NULL;
  foreach(QtProperty* prop, d->VariantManager->properties())
    {
    if (prop->propertyName() == id)
      {
      param = dynamic_cast<QtVariantProperty*>(prop);
      break;
      }
    }

  if (!param)
    {
    param = d->VariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), id);
    param->setPropertyId(id);
    }

  param->setAttribute(QLatin1String("enumNames"), choices);
  param->setValue(choices.indexOf(value));

  return param;
}

// --------------------------------------------------------------------------
QString voAnalysis::stringParameter(const QString& id)const
{
  QtVariantProperty * prop = this->parameter(id);
  Q_ASSERT(prop);
  return prop->value().toString();
}

// --------------------------------------------------------------------------
QtVariantProperty*  voAnalysis::addStringParameter(const QString& id, const QString& label,
                                                   const QString& value)
{
  Q_ASSERT(!id.isEmpty());
  Q_ASSERT(!label.isEmpty());

  Q_D(voAnalysis);

  QtVariantProperty * param = d->VariantManager->addProperty(QVariant::String, label);

  param->setPropertyId(id);
  param->setValue(value);

  return param;
}

// --------------------------------------------------------------------------
vtkTree* voAnalysis::treeParameter(const QString& label)const
{
  QString treeName = this->enumParameter(label);
  qDebug() << "Selected tree:" << treeName;

  voDataModelItem* treeItem =
    voApplication::application()->dataModel()->findItemWithText(treeName);
  Q_ASSERT(treeItem);

  vtkTree *tree =
    vtkTree::SafeDownCast(treeItem->dataObject()->dataAsVTKDataObject());

  return tree;
}

// --------------------------------------------------------------------------
void voAnalysis::addTreeParameter(const QString& label)
{
  Q_D(voAnalysis);

  QPair<QString, QString> pair;
  pair.first = "vtkTree";
  pair.second = label;

  d->DynamicParameters << pair;
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
  Q_ASSERT(!id.isEmpty());
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

// --------------------------------------------------------------------------
void voAnalysis::updateDynamicParameters()
{
  typedef QPair<QString, QString> DynamicPropertyType;
  foreach(const DynamicPropertyType dynamicProp, this->dynamicParameters())
    {
    QString type = dynamicProp.first;
    QString label = dynamicProp.second;
    QStringList choices;
    voApplication::application()->dataModel()->listItems(type, &choices);
    this->updateEnumParameter(label, choices);
    }
}

// --------------------------------------------------------------------------
vtkSmartPointer<vtkExtendedTable> voAnalysis::getInputTable() const
{
  Q_D(const voAnalysis);

  vtkSmartPointer<vtkExtendedTable> extendedTable;

  for(int i = 0; i < d->InputDataObjects.size(); ++i)
    {
    extendedTable = this->getInputTable(i);
    if (extendedTable)
      {
      return extendedTable;
      }
    }
  return NULL;
}

vtkSmartPointer<vtkExtendedTable> voAnalysis::getInputTable(int i) const
{
  Q_D(const voAnalysis);

  vtkSmartPointer<vtkExtendedTable> extendedTable;
  voDataObject *inputDataObject = d->InputDataObjects.at(i).data();

  extendedTable =
    vtkExtendedTable::SafeDownCast(inputDataObject->dataAsVTKDataObject());
  if (extendedTable)
    {
    return extendedTable;
    }

  vtkTable *table =
    vtkTable::SafeDownCast(inputDataObject->dataAsVTKDataObject());
  if (table)
    {
    extendedTable = vtkSmartPointer<vtkExtendedTable>::New();
    voIOManager::convertTableToExtended(table, extendedTable.GetPointer());
    return extendedTable;
    }

  return NULL;
}
