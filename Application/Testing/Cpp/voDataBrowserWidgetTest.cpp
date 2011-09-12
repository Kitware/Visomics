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

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDelimitedTextReader.h>
#include <vtkStringToNumeric.h>

// Visomics includes
#include <voDataBrowserWidget.h>
#include <voDataModel.h>
#include <voDataModelItem.h>
#include <voInputFileDataObject.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int voDataBrowserWidgetTest(int argc, char * argv [])
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

  voDataBrowserWidget w;
  voDataModel model;
  w.setModel(&model);
  w.setSelectionModel(model.selectionModel());

  w.show();

  // Read file
  QString filename(argv[1]);
  vtkSmartPointer<vtkDelimitedTextReader> reader = vtkSmartPointer<vtkDelimitedTextReader>::New();
  reader->SetFieldDelimiterCharacters(",");
  reader->SetHaveHeaders(1);
  reader->DetectNumericColumnsOff();
  reader->SetFileName(filename.toAscii().data());
  reader->Update();

  // Detect numeric columns
  vtkSmartPointer<vtkStringToNumeric> NumericOutput = vtkSmartPointer<vtkStringToNumeric>::New();
  NumericOutput->SetInputConnection(reader->GetOutputPort());
  NumericOutput->Update();

  // Add corresponding dataObject to model
  voInputFileDataObject * dataObject = new voInputFileDataObject();
  dataObject->setName("test");
  dataObject->setFileName(filename);
  dataObject->setData(NumericOutput->GetOutput());
  model.addDataObject(dataObject);

  QStandardItem * container1 = model.addContainer("PCA");
  Q_ASSERT(container1);
  QStandardItem * container2 = model.addContainer("PCA2");
  Q_ASSERT(container2);

  model.addContainer("Outputs", container1);

  model.addContainer("Views", container1);

  model.addContainer("Outputs", container2);

  model.addContainer("Views", container2);

  QTimer autoExit;
  //if (argc < 2 || QString(argv[1]) != "-I")
  //  {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
  //  }
  return app.exec();
}

