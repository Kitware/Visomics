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
#include "voInputFileDataObject.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkCompositeDataIterator.h>
#include <vtkDoubleArray.h>
#include <vtkGraph.h>
#include <vtkMultiPieceDataSet.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToGraph.h>
#include <vtkTree.h>

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
      this->addOutputType(name, "vtkTable",
                          "", "",
                          "voTableView", name);
      }
    else if (type == "Tree")
      {
      this->addOutputType(name, "vtkTree",
                          "", "",
                          "voTreeHeatmapView", name);
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
        this->parameterDescriptions.append(
          QString("<dt><b>%1:</b></dt>").arg(parameterTitle));
        this->parameterDescriptions.append(
          QString("<dd>%1</dd>").arg(field->value()));
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
      qCritical() << "Unsupported parameter type in voCustomAnalysis:" << type;
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

  d->RCalc->SetRoutput(0);

  vtkSmartPointer<vtkExtendedTable> extendedTable;
  vtkNew<vtkMultiPieceDataSet> composite;
  bool multiInput = false;

  if (d->Information->inputs().size() > 1)
    {
    multiInput = true;
    composite->SetNumberOfPieces(d->Information->inputs().size());
    }

  int itr = 0;
  foreach(voCustomAnalysisData *input, d->Information->inputs())
    {
    if (input->type() == "Table")
      {
      extendedTable = this->getInputTable(itr);
      vtkTable *table = extendedTable->GetInputData();
      d->RCalc->PutTable(input->name().toStdString().c_str());
      if (multiInput)
        {
        composite->SetPiece(itr, table);
        }
      else
        {
        d->RCalc->SetInputData(table);
        }
      }

    else if(input->type() == "Tree")
      {
      vtkTree* tree =
        vtkTree::SafeDownCast(this->input(itr)->dataAsVTKDataObject());
      if (!tree)
        {
        qCritical() << "Input Tree is Null";
        return false;
        }
      d->RCalc->PutTree(input->name().toStdString().c_str());
      if (multiInput)
        {
        composite->SetPiece(itr, tree);
        }
      else
        {
        d->RCalc->SetInputData(tree);
        }
      }
    else
      {
      qCritical() << "Unsupported input type:" << input->type();
      return false;
      }
    ++itr;
    }
  if (multiInput)
    {
    d->RCalc->SetInputData(composite.GetPointer());
    }

  foreach(voCustomAnalysisData *output, d->Information->outputs())
    {
    const char *outputName = output->name().toStdString().c_str();
    if (output->type() == "Table")
      {
      d->RCalc->GetTable(outputName);
      }
    else if(output->type() == "Tree")
      {
      d->RCalc->GetTree(outputName);
      }
    else
      {
      qCritical() << "Unsupported output type:" << output->type();
      return false;
      }
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
      parameterValue = QString("\"%1\"").arg(this->stringParameter(name));
      }
    else
      {
      qCritical() << "Unsupported parameter type in voCustomAnalysis:" << type;
      }
    script.replace(name, parameterValue);
    }

  d->RCalc->SetRscript(script.toLatin1());
  d->RCalc->Update();

  // Get output(s) from R
  if (multiInput)
    {
    vtkMultiPieceDataSet * outData =
      vtkMultiPieceDataSet::SafeDownCast(d->RCalc->GetOutput());
    if(!outData)
      {
      qCritical() << QObject::tr("Fatal error running analysis");
      return false;
      }

    vtkCompositeDataIterator* iter = outData->NewIterator();
    itr = 0;
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal();
         iter->GoToNextItem())
      {
      voCustomAnalysisData *output = d->Information->outputs().at(itr);
      if (output->type() == "Table")
        {
        vtkTable *outputTable =
          vtkTable::SafeDownCast(iter->GetCurrentDataObject());
        this->setOutput(output->name(),
          new voTableDataObject(output->name(), outputTable, true));
        }
      else // tree is the only other possibility atm
        {
        vtkTree *outputTree = vtkTree::SafeDownCast(iter->GetCurrentDataObject());
        this->setOutput(output->name(),
                        new voInputFileDataObject(output->name(), outputTree));
        }
      ++itr;
      }
    iter->Delete();
    }
  else
    {
    foreach(voCustomAnalysisData *output, d->Information->outputs())
      {
      if (output->type() == "Table")
        {
        vtkTable * outputTable = vtkTable::SafeDownCast(d->RCalc->GetOutput());
        this->setOutput(output->name(),
                        new voTableDataObject(output->name(), outputTable));
        }
      else
        {
        vtkTree * outputTree = vtkTree::SafeDownCast(d->RCalc->GetOutput());
        this->setOutput(output->name(),
                        new voDataObject(output->name(), outputTree));
        }
      }
    }
  return true;
}
