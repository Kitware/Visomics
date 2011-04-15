#ifndef __voDelimitedTextImportSettings_h
#define __voDelimitedTextImportSettings_h

// Qt includes
#include <QHash>
#include <QString>

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
    HaveHeaders,
    // vtkExtendedTable settings
    Transpose,
    NumberOfColumnMetaDataTypes, 
    ColumnMetaDataTypeOfInterest,
    NumberOfRowMetaDataTypes,
    RowMetaDataTypeOfInterest,
    };

  voDelimitedTextImportSettings();

  virtual void printAdditionalInfo()const;
    
  void setDefaultSettings();
};


#endif
