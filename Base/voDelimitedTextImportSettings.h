#ifndef __voDelimitedTextImportSettings_h
#define __voDelimitedTextImportSettings_h

// Qt includes
#include <QHash>
#include <QString>

class voDelimitedTextImportSettings : QHash<int, QVariant>
{
public:
  typedef voDelimitedTextImportSettings Self;

  enum DelimitedTextReaderSettings
    {
    // vtkDelimitedTextReader settings
    FieldDelimiter = 0,
    MergeConsecutiveDelimiters,
    StringDelimiter,
    UseStringDelimiter,
    HaveHeaders,
    Transpose, 
    NumberOfColumnMetaDataTypes, 
    ColumnMetaDataTypeOfInterest,
    NumberOfRowMetaDataTypes,
    RowMetaDataTypeOfInterest,
    };

  voDelimitedTextImportSettings();
    
  void setDefaultSettings();
};


#endif
