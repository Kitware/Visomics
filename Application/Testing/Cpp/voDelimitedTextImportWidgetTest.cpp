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
#include <QApplication>
#include <QTimer>
#include <QDebug>

// Visomics includes
#include <voDelimitedTextPreview.h>
#include <voDelimitedTextImportWidget.h>
#include <voDelimitedTextImportSettings.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int voDelimitedTextImportWidgetTest(int argc, char * argv [])
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(VisomicsApp);
  
  if (argc < 2)
    {
    // TODO Add a better to handle arguments. May be by using ctkCommandLineParser
    std::cerr << "Missing an argument !\n"
              << "Usage: \n"
              << "  " << argv[0] << " </path/to/data.csv>" << std::endl;
    return EXIT_FAILURE;
    }

  // Read file
  QString filename(argv[1]);

  voDelimitedTextImportSettings defaultSettings;
  voDelimitedTextPreviewModel model(defaultSettings);
  model.setFileName(filename);

  voDelimitedTextPreview * documentPreviewWidget = new voDelimitedTextPreview;
  documentPreviewWidget->setModel(&model);

  voDelimitedTextImportWidget w;
  w.setDelimitedTextPreviewModel(&model);
  w.insertWidget(documentPreviewWidget, voDelimitedTextImportWidget::RowsAndColumnsGroupBox);
  w.show();

  QTimer autoExit;
  //if (argc < 2 || QString(argv[1]) != "-I")
  //  {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
  //  }
  return app.exec();
}

