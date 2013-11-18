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

#include "voCustomAnalysisData.h"

// --------------------------------------------------------------------------
voCustomAnalysisData::voCustomAnalysisData(QObject* newParent): Superclass(newParent)
{
}

// --------------------------------------------------------------------------
voCustomAnalysisData::~voCustomAnalysisData()
{
}

// --------------------------------------------------------------------------
QString voCustomAnalysisData::name() const
{
  return this->dataName;
}

// --------------------------------------------------------------------------
QString voCustomAnalysisData::type() const
{
  return this->dataType;
}

// --------------------------------------------------------------------------
QString voCustomAnalysisData::viewType() const
{
  return this->viewDataType;
}

// --------------------------------------------------------------------------
void voCustomAnalysisData::setName(const QString& name)
{
  this->dataName = name;
}

// --------------------------------------------------------------------------
void voCustomAnalysisData::setType(const QString& type)
{
  this->dataType = type;
}

// --------------------------------------------------------------------------
void voCustomAnalysisData::setViewType(const QString& type)
{
  this->viewDataType = type;
}


// --------------------------------------------------------------------------
void voCustomAnalysisData::setDataIsInput(bool b)
{
  this->isInput = b;
}
