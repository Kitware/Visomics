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
#ifndef __voDelimitedTextImportSettings_h
#define __voDelimitedTextImportSettings_h

// Qt includes
#include <QHash>
#include <QString>
#include <QVariant>

class voDelimitedTextImportSettings : public QHash<int, QVariant>
{
public:
  typedef voDelimitedTextImportSettings Self;

  enum DelimitedTextReaderSettings
    {
    // vtkDelimitedTextReader settings
    FieldDelimiterCharacters = 0,
    MergeConsecutiveDelimiters,
    StringDelimiter,
    UseStringDelimiter,
    // vtkExtendedTable settings
    Transpose,
    NumberOfColumnMetaDataTypes, 
    ColumnMetaDataTypeOfInterest,
    NumberOfRowMetaDataTypes,
    RowMetaDataTypeOfInterest,
    // Normalization settings
    NormalizationMethod,
    };

  voDelimitedTextImportSettings();

  virtual void printAdditionalInfo()const;
    
  void setDefaultSettings();
};


#endif
