
// Qt includes
#include <QVariant>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

// --------------------------------------------------------------------------
voDelimitedTextImportSettings::voDelimitedTextImportSettings()
{
  this->setDefaultSettings();
}

// --------------------------------------------------------------------------
void voDelimitedTextImportSettings::setDefaultSettings()
{
  this->insert(Self::FieldDelimiter, ",");
  this->insert(Self::MergeConsecutiveDelimiters, false);
  this->insert(Self::StringDelimiter, "\"");
  this->insert(Self::UseStringDelimiter, true);
  this->insert(Self::HaveHeaders, false);
  this->insert(Self::Transpose, false);
  this->insert(Self::NumberOfColumnMetaDataTypes, 1);
  this->insert(Self::ColumnMetaDataTypeOfInterest, 0);
  this->insert(Self::NumberOfRowMetaDataTypes, 1);
  this->insert(Self::RowMetaDataTypeOfInterest, 0);
}
