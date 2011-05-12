

#ifndef __voIOManager_h
#define __voIOManager_h

// Qt includes
#include <QString>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

class vtkExtendedTable;
class vtkTable;
class voIOManager
{
public:
  typedef voIOManager Self;

  static void readCSVFileIntoTable(const QString& fileName, vtkTable * outputTable,
                                   const voDelimitedTextImportSettings& settings = voDelimitedTextImportSettings());

  static void fillExtendedTable(vtkTable* sourceTable, vtkExtendedTable* destTable,
                                const voDelimitedTextImportSettings& settings);

  void openCSVFile(const QString& fileName, const voDelimitedTextImportSettings& settings);
  
};

#endif
