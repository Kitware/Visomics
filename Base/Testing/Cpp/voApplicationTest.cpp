
// Qt includes
#include <QStringList>

// Visomics includes
#include "voApplication.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int voApplicationTest(int argc, char * argv [])
{
  voApplication app(argc, argv);

  if (app.isInstalled())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voApplication::isInstalled" << std::endl;
    return EXIT_FAILURE;
    }

  if (app.arguments().size() < 2)
    {
    std::cerr << "Line " << __LINE__
              << " - Expected home directory should be passed as first argument" << std::endl;
    return EXIT_FAILURE;
    }

  QString expectedHomeDirectory(app.arguments().at(1));
  if (app.homeDirectory() != expectedHomeDirectory)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voApplication::homeDirectory\n"
              << "\tExpected home directory:" << qPrintable(expectedHomeDirectory) << "\n"
              << "\tCurrent home directory:" << qPrintable(app.homeDirectory()) << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
