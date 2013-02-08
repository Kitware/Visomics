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

// QtPropertyBrowser includes
#include <QtVariantProperty>
#include <QtVariantPropertyManager>

// Visomics includes
#include "voCustomAnalysis.h"
#include "voCustomAnalysisInformation.h"
#include "voDataObject.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkDoubleArray.h>
#include <vtkGraph.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToGraph.h>

// --------------------------------------------------------------------------
// voCustomAnalysisPrivate methods

// --------------------------------------------------------------------------
class voCustomAnalysisPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
  voCustomAnalysisInformation *Information;
};

// --------------------------------------------------------------------------
// voCustomAnalysis methods

// --------------------------------------------------------------------------
voCustomAnalysis::voCustomAnalysis(QObject* newParent):
    Superclass(), d_ptr(new voCustomAnalysisPrivate)
{
  if (newParent)
    {
    this->setParent(newParent);
    }
  Q_D(voCustomAnalysis);
  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
  d->Information = NULL;
}

// --------------------------------------------------------------------------
voCustomAnalysis::~voCustomAnalysis()
{
}

// --------------------------------------------------------------------------
void voCustomAnalysis::loadInformation(voCustomAnalysisInformation *info)
{
  Q_D(voCustomAnalysis);
  d->Information = info;
  this->inputName = d->Information->input()->name();
  this->inputType = d->Information->input()->name();
}

// --------------------------------------------------------------------------
void voCustomAnalysis::setOutputInformation()
{
  Q_D(voCustomAnalysis);
  if (d->Information == NULL)
    {
    return;
    }
  foreach(voCustomAnalysisData *output, d->Information->outputs())
    {
    QString name = output->name();
    QString type = output->type();
    if (type == "Table")
      {
      this->addOutputType(name, "vtkTable", "voTableView", name);
      }
    else if (type == "Tree")
      {
      this->addOutputType(name, "vtkTree", "voTreeHeatmapView", name);
      }
    else
      {
      qCritical() << "Unsupported output type in voCustomAnalysis:" << type;
      }
    }
}

// --------------------------------------------------------------------------
void voCustomAnalysis::setParameterInformation()
{
  Q_D(voCustomAnalysis);
  if (d->Information == NULL)
    {
    return;
    }

  // first handle the case where this custom analysis has no parameters.
  if (d->Information->parameters().size() == 0)
    {
    this->parameterDescriptions = "";
    return;
    }

  QList<QtProperty*> custom_parameters;
  this->parameterDescriptions = "<dl>";
  foreach(voCustomAnalysisParameter *parameter, d->Information->parameters())
    {
    QString type = parameter->type();
    QString name = parameter->name();
    QString parameterTitle = "";
    QStringList enum_options;

    QVariant minValue(VTK_INT_MIN);
    QVariant maxValue(VTK_INT_MAX);
    QVariant defaultValue(0);

    // read fields for this parameter
    foreach(voCustomAnalysisParameterField *field, parameter->fields())
      {
      if (field->name() == "title")
        {
        parameterTitle = field->value();
        }
      else if (field->name() == "description")
        {
        this->parameterDescriptions.append(QString("<dt><b>%1:</b></dt>").arg(name));
        this->parameterDescriptions.append(QString("<dd>%1</dd>").arg(field->value()));
        }
      else if (field->name() == "min")
        {
        minValue = QVariant(field->value().toDouble());
        }
      else if (field->name() == "max")
        {
        maxValue = QVariant(field->value().toDouble());
        }
      else if (field->name() == "default")
        {
        if (type == "String")
          {
          defaultValue = QVariant(field->value());
          }
        else
          {
          defaultValue = QVariant(field->value().toDouble());
          }
        }
      else if (field->name() == "option")
        {
        enum_options << field->value();
        }
      }

    // set up this parameter, based on what type it is
    if (type == "Integer")
      {
      custom_parameters << this->addIntegerParameter(
        parameter->name(), parameterTitle,
        minValue.toInt(), maxValue.toInt(), defaultValue.toInt());
      }
    else if (type == "Double")
      {
      custom_parameters << this->addIntegerParameter(
        parameter->name(), parameterTitle,
        minValue.toDouble(), maxValue.toDouble(), defaultValue.toDouble());
      }
    else if (type == "String")
      {
      custom_parameters << this->addStringParameter(
        parameter->name(), parameterTitle, defaultValue.toString());
      }
    else if (type == "Enum")
      {
      custom_parameters << this->addEnumParameter(
        parameter->name(), parameterTitle, enum_options);
      }
    else
      {
      qCritical() << "Unsupported output type in voCustomAnalysis:" << type;
      }
    }
  this->parameterDescriptions.append("</dl>");

  this->addParameterGroup(QString("%1 parameters").arg(this->objectName()),
                          custom_parameters);
}

// --------------------------------------------------------------------------
QString voCustomAnalysis::parameterDescription()const
{
  return this->parameterDescriptions;
}

// --------------------------------------------------------------------------
bool voCustomAnalysis::execute()
{
  Q_D(voCustomAnalysis);

  // Import data table locally
  vtkSmartPointer<vtkExtendedTable> extendedTable = this->getInputTable();
  if (!extendedTable)
    {
    qCritical() << "Input is Null";
    return false;
    }

  vtkSmartPointer<vtkTable> inputDataTable = extendedTable->GetData();

  // Build ArrayData for input to R
  vtkNew<vtkArrayData> RInputArrayData;
    {
    vtkSmartPointer<vtkArray> RInputArray;
    voUtils::tableToArray(inputDataTable.GetPointer(), RInputArray);
    RInputArrayData->AddArray(RInputArray.GetPointer());
    }

  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputData(RInputArrayData.GetPointer());
  d->RCalc->PutArray("0", this->inputName.toStdString().c_str());

  foreach(voCustomAnalysisData *output, d->Information->outputs())
    {
    const char *outputName = output->name().toStdString().c_str();
    d->RCalc->GetArray(outputName, outputName);
    }

  // replace each parameter in the script with its actual value
  QString script = d->Information->script();
  foreach(voCustomAnalysisParameter *parameter, d->Information->parameters())
    {
    QString type = parameter->type();
    QString name = parameter->name();
    QString parameterValue;
    if (type == "Integer")
      {
      parameterValue = QString::number(this->integerParameter(name));
      }
    else if (type == "Double")
      {
      parameterValue = QString::number(this->doubleParameter(name));
      }
    else if (type == "Enum")
      {
      parameterValue = this->enumParameter(name);
      }
    else if (type == "String")
      {
      parameterValue = this->stringParameter(name);
      }
    else
      {
      qCritical() << "Unsupported parameter type in voCustomAnalysis:" << type;
      }
    script.replace(name, parameterValue);
    }

  d->RCalc->SetRscript(script.toStdString().c_str());
  d->RCalc->Update();

  // Get output(s) from R
  foreach(voCustomAnalysisData *output, d->Information->outputs())
    {
    if (output->type() == "Table")
      {
      vtkSmartPointer<vtkArrayData> outputArrayData =
        vtkArrayData::SafeDownCast(d->RCalc->GetOutput());
      if (!outputArrayData || !outputArrayData->GetArrayByName(output->name().toStdString().c_str()))
        {
        qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
        return false;
        }

      // Get row labels
      vtkNew<vtkStringArray> rowLabels;
      voUtils::addCounterLabels(
        extendedTable->GetRowMetaDataOfInterestAsString(),
        rowLabels.GetPointer(), false);

      // Extract output table
      vtkNew<vtkTable> outputTable;
        {
        voUtils::arrayToTable(outputArrayData->GetArrayByName(output->name().toStdString().c_str()),
                              outputTable.GetPointer());
        for (vtkIdType c = 0;c < outputTable->GetNumberOfColumns(); ++c)
          {
          outputTable->GetColumn(c)->SetName(rowLabels->GetValue(c));
          }
        voUtils::insertColumnIntoTable(outputTable.GetPointer(),
                                       0, rowLabels.GetPointer());
        }

      this->setOutput(output->name(),
        new voTableDataObject(output->name(), outputTable.GetPointer(), true));
      }
    else
      {
      qCritical() << "Only table output is supported at the moment";
      }
    }
  return true;
}
