

#ifndef __voIOManager_h
#define __voIOManager_h

// Qt includes
#include <QString>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

class voIOManager
{
public:

  void openCSVFile(const QString& fileName, const voDelimitedTextImportSettings& settings);
  
};

#endif
