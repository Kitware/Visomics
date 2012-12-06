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
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTemporaryFile>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisFactory.h"
#include "voDelimitedTextImportSettings.h"
#include "voInputFileDataObject.h"
#include "voIOManager.h"
#include "voTestConfigure.h"

// VTK includes
#include <vtkExtendedTable.h>
#include <vtkNew.h>
#include <vtkTable.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int voAnalysisRunTest(int argc, char * argv [])
{
  QApplication app(argc, argv);

  if (argc < 2)
    {
    std::cerr << "Usage:\n\n"
              << "  " << argv[0] << " <AnalysisName> [-I]\n"
              << "\nOptions:\n"
              << "\n  -I If specified, the baseline output file will be generated\n"
              << "\n  -K If specified, the temporary output generated won't be deleted\n" << std::endl;
    return EXIT_FAILURE;
    }

  // Extract test parameters
  QString dataDirectory(VISOMICS_DATA_DIR);
  QString fileName("Data/sample/visomics-sample_liver.csv");
  QString analysisName(argv[1]);
  bool generateOutputBaselines = app.arguments().contains("-I");
  bool keepTemporaryOutput = app.arguments().contains("-K");

  if (QFileInfo(fileName).isRelative())
    {
    fileName.prepend("/").prepend(dataDirectory);
    }

  // Sanity checks
  if (!QFile::exists(fileName))
    {
    std::cerr << "Line " << __LINE__ << " - "
                 "Filename "<< qPrintable(fileName) << " doesn't exist !" << std::endl;
    return EXIT_FAILURE;
    }

  // Import settings
  voDelimitedTextImportSettings importSettings;
  importSettings.insert(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes, 4);

  // Compute MD5 of input file
  QFile inputFile(fileName);
  if (!inputFile.open(QIODevice::ReadOnly))
    {
    std::cerr << "Line " << __LINE__ << " - "
                 "Failed to open file "<< qPrintable(fileName) << std::endl;
    return EXIT_FAILURE;
    }
  QString inputMD5 = QCryptographicHash::hash(inputFile.readAll(), QCryptographicHash::Md5).toHex();
  inputFile.close();

  // Read data into 'raw' table
  vtkNew<vtkTable> table;
  voIOManager::readCSVFileIntoTable(fileName, table.GetPointer(), importSettings);

  // Fill the 'extended table' using the 'raw' table and the import settings
  vtkNew<vtkExtendedTable> extendedTable;
  voIOManager::fillExtendedTable(table.GetPointer(), extendedTable.GetPointer(), importSettings);

  // Instantiate analysis
  voAnalysisFactory factory;
  voAnalysis* analysis = factory.createAnalysis(analysisName);
  if (!analysis)
    {
    std::cerr << "Line " << __LINE__ << " - "
                 "Failed to create analysis "<< qPrintable(analysisName) << " !" << std::endl;
    return EXIT_FAILURE;
    }

  // Initialize analysis
  analysis->initializeOutputInformation();
  analysis->initializeParameterInformation();
  analysis->addInput(new voInputFileDataObject(fileName, extendedTable.GetPointer()));
  analysis->setWriteOutputsToFilesEnabled(generateOutputBaselines);
  dataDirectory.append("/Baseline/Base/Analysis/");
  analysis->setOutputDirectory(dataDirectory);

  // Run analysis
  bool success = analysis->run();
  if (!success)
    {
    std::cerr << "Line " << __LINE__ << " - "
                 "Failed to run analysis "<< qPrintable(analysisName) << " !" << std::endl;
    return EXIT_FAILURE;
    }

  if (generateOutputBaselines)
    {
    std::cout << "Baselines generated: " << qPrintable(analysisName) << std::endl;
    return EXIT_SUCCESS;
    }

  // Compare analysis outputs with baselines
  foreach(const QString& outputName, analysis->outputNames())
    {
    // Special case: ignore non-VTK outputs
    if(!analysis->output(outputName)->isVTKDataObject())
      {
      continue;
      }

    // Baseline file
    QString fileName = inputMD5 + "_" + analysisName + "_" + outputName + ".vtk";
    fileName.prepend(dataDirectory);
    if (!QFile::exists(fileName))
      {
      std::cerr << "Line " << __LINE__ << " - "
                   "Baseline file do NOT exist:" << qPrintable(fileName) << std::endl;
      return EXIT_FAILURE;
      }

    // Read file and compute hash
    QFile baselineFile(fileName);
    if (!baselineFile.open(QIODevice::ReadOnly))
      {
      std::cerr << "Line " << __LINE__ << " - "
                   "Failed to open baseline file:" << qPrintable(fileName) << std::endl;
      return EXIT_FAILURE;
      }
    QByteArray baselineMD5 = QCryptographicHash::hash(baselineFile.readAll(), QCryptographicHash::Md5);

    // Output file
    QTemporaryFile tempFile;
    if(keepTemporaryOutput)
      {
      tempFile.setAutoRemove(false);
      }
    if (!tempFile.open())
      {
      std::cerr << "Line " << __LINE__ << " - "
                   "Failed to create temporary file" << std::endl;
      return EXIT_FAILURE;
      }
    voDataObject * dataObject = analysis->output(outputName);
    voIOManager::writeDataObjectToFile(dataObject->dataAsVTKDataObject(), tempFile.fileName().toLatin1());

    QByteArray outputMD5 = QCryptographicHash::hash(tempFile.readAll(), QCryptographicHash::Md5);

    if (baselineMD5 != outputMD5)
      {
      std::cerr << "Line " << __LINE__ << " - MD5 do NOT match !\n"
                << "\tBaseline MD5: " << qPrintable(QString(baselineMD5.toHex()))
                << ", file: " << qPrintable(fileName) <<"\n"
                << "\tOutput MD5: " << qPrintable(QString(outputMD5.toHex()))
                << ", file: " << qPrintable(tempFile.fileName()) << std::endl;
      return EXIT_FAILURE;
      }
    }

  delete analysis;

  return EXIT_SUCCESS;
}
