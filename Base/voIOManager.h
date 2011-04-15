

#ifndef __voIOManager_h
#define __voIOManager_h

// Qt includes
#include <QStringList>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

class voIOManager
{
public:

  void openCSVFiles(const QStringList& fileNames);

  void openCSVFile(const QString& fileName, const voDelimitedTextImportSettings& settings);
  
};

#endif
