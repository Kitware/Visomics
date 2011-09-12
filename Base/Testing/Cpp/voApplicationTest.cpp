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
