
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
}
