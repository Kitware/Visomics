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

#include "voCustomAnalysisParameter.h"

// --------------------------------------------------------------------------
voCustomAnalysisParameter::voCustomAnalysisParameter(QObject* newParent): Superclass(newParent)
{
}

// --------------------------------------------------------------------------
voCustomAnalysisParameter::~voCustomAnalysisParameter()
{
}

// --------------------------------------------------------------------------
QString voCustomAnalysisParameter::name() const
{
  return this->parameterName;
}

// --------------------------------------------------------------------------
QString voCustomAnalysisParameter::type() const
{
  return this->parameterType;
}

// --------------------------------------------------------------------------
voCustomAnalysisParameterField * voCustomAnalysisParameter::field(int i) const
{
  return this->fieldsList.at(i);
}

// --------------------------------------------------------------------------
QList<voCustomAnalysisParameterField *> voCustomAnalysisParameter::fields() const
{
  return this->fieldsList;
}

// --------------------------------------------------------------------------
void voCustomAnalysisParameter::setName(const QString& name)
{
  this->parameterName = name;
}

// --------------------------------------------------------------------------
void voCustomAnalysisParameter::setType(const QString& type)
{
  this->parameterType = type;
}

// --------------------------------------------------------------------------
void voCustomAnalysisParameter::addField(voCustomAnalysisParameterField * field)
{
  this->fieldsList.append(field);
}
