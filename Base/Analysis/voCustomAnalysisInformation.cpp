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

#include "voCustomAnalysisInformation.h"

// --------------------------------------------------------------------------
voCustomAnalysisInformation::voCustomAnalysisInformation(QObject* newParent): Superclass(newParent)
{
}

// --------------------------------------------------------------------------
voCustomAnalysisInformation::~voCustomAnalysisInformation()
{
}

// --------------------------------------------------------------------------
QString voCustomAnalysisInformation::name() const
{
  return this->analysisName;
}

// --------------------------------------------------------------------------
void voCustomAnalysisInformation::setName(const QString& name)
{
  this->analysisName = name;
}

// --------------------------------------------------------------------------
QString voCustomAnalysisInformation::script() const
{
  return this->analysisScript;
}

// --------------------------------------------------------------------------
void voCustomAnalysisInformation::setScript(const QString& script)
{
  this->analysisScript = script;
}

// --------------------------------------------------------------------------
voCustomAnalysisData * voCustomAnalysisInformation::input(int i) const
{
  return this->inputsList.at(i);
}

// --------------------------------------------------------------------------
voCustomAnalysisData * voCustomAnalysisInformation::output(int i) const
{
  return this->outputsList.at(i);
}

// --------------------------------------------------------------------------
QList<voCustomAnalysisData *> voCustomAnalysisInformation::inputs() const
{
  return this->inputsList;
}

// --------------------------------------------------------------------------
QList<voCustomAnalysisData *> voCustomAnalysisInformation::outputs() const
{
  return this->outputsList;
}

// --------------------------------------------------------------------------
voCustomAnalysisParameter * voCustomAnalysisInformation::parameter(int i) const
{
  return this->parametersList.at(i);
}

// --------------------------------------------------------------------------
QList<voCustomAnalysisParameter *> voCustomAnalysisInformation::parameters() const
{
  return this->parametersList;
}

// --------------------------------------------------------------------------
void voCustomAnalysisInformation::addInput(voCustomAnalysisData * input)
{
  this->inputsList.append(input);
}

// --------------------------------------------------------------------------
void voCustomAnalysisInformation::addOutput(voCustomAnalysisData * output)
{
  this->outputsList.append(output);
}

// --------------------------------------------------------------------------
void voCustomAnalysisInformation::addParameter(voCustomAnalysisParameter * parameter)
{
  this->parametersList.append(parameter);
}
