

#ifndef __voIOManager_h
#define __voIOManager_h

// Qt includes
#include <QString>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

class vtkDataObject;
class vtkExtendedTable;
class vtkTable;

class voIOManager
{
public:
  typedef voIOManager Self;

  static bool readCSVFileIntoTable(const QString& fileName, vtkTable * outputTable,
                                   const voDelimitedTextImportSettings& settings = voDelimitedTextImportSettings());

  static bool writeTableToCVSFile(vtkTable* table, const QString& fileName);

  static void fillExtendedTable(vtkTable* sourceTable, vtkExtendedTable* destTable,
                                const voDelimitedTextImportSettings& settings = voDelimitedTextImportSettings());

  void openCSVFile(const QString& fileName, const voDelimitedTextImportSettings& settings);

  static bool writeDataObjectToFile(vtkDataObject * dataObject, const QString& fileName);
  
};

#endif
