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
#include <QVariant>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

// --------------------------------------------------------------------------
voDelimitedTextImportSettings::voDelimitedTextImportSettings()
{
  this->setDefaultSettings();
}

// --------------------------------------------------------------------------
void voDelimitedTextImportSettings::printAdditionalInfo()const
{
  qDebug() << "voDelimitedTextImportSettings (" << this << ")" << endl
           << " FieldDelimiter:" << this->value(Self::FieldDelimiterCharacters).toString() << endl
           << " MergeConsecutiveDelimiters:" << this->value(Self::MergeConsecutiveDelimiters).toBool() << endl
           << " StringDelimiter:" << this->value(Self::StringDelimiter).toChar() << endl
           << " UseStringDelimiter:" << this->value(Self::UseStringDelimiter).toBool() << endl
           << " Transpose:" << this->value(Self::Transpose).toBool() << endl
           << " NumberOfColumnMetaDataTypes:" << this->value(Self::NumberOfColumnMetaDataTypes).toInt() << endl
           << " ColumnMetaDataTypeOfInterest:" << this->value(Self::ColumnMetaDataTypeOfInterest).toInt() << endl
           << " NumberOfRowMetaDataTypes:" << this->value(Self::NumberOfRowMetaDataTypes).toInt() << endl
           << " RowMetaDataTypeOfInterest:" << this->value(Self::RowMetaDataTypeOfInterest).toInt();
}

// --------------------------------------------------------------------------
void voDelimitedTextImportSettings::setDefaultSettings()
{
  this->insert(Self::FieldDelimiterCharacters, ",");
  this->insert(Self::MergeConsecutiveDelimiters, false);
  this->insert(Self::StringDelimiter, '\"');
  this->insert(Self::UseStringDelimiter, true);
  this->insert(Self::Transpose, false);
  this->insert(Self::NumberOfColumnMetaDataTypes, 1);
  this->insert(Self::ColumnMetaDataTypeOfInterest, 0);
  this->insert(Self::NumberOfRowMetaDataTypes, 1);
  this->insert(Self::RowMetaDataTypeOfInterest, 0);
  this->insert(Self::NormalizationMethod, "No");
}
