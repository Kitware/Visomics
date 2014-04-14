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
  this->dataIncludeMetadata = true;
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
bool voCustomAnalysisData::includeMetadata() const
{
  return this->dataIncludeMetadata;
}

// --------------------------------------------------------------------------
QList< QPair< QString, QString> > voCustomAnalysisData::views() const
{
  return this->dataViews;
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
void voCustomAnalysisData::addView(const QString& name, const QString& type)
{
  this->dataViews.append(QPair<QString, QString>(name, type));
}

// --------------------------------------------------------------------------
void voCustomAnalysisData::setIncludeMetadata(bool b)
{
  this->dataIncludeMetadata = b;
}
