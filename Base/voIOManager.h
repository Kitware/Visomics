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


#ifndef __voIOManager_h
#define __voIOManager_h

// Qt includes
#include <QString>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

class QStandardItem;
class QXmlStreamReader;
class QXmlStreamWriter;
class voDataModelItem;
class voInputFileDataObject;
class vtkDataObject;
class vtkExtendedTable;
class vtkTable;
class vtkTree;

class voIOManager
{
public:
  typedef voIOManager Self;

  static bool readCSVFileIntoTable(const QString& fileName, vtkTable * outputTable,
                                   const voDelimitedTextImportSettings& settings = voDelimitedTextImportSettings(), const bool haveHeaders = false);
  static bool readCSVFileIntoExtendedTable(const QString& fileName,
                                           vtkExtendedTable *outputTable,
                                           const voDelimitedTextImportSettings& settings = voDelimitedTextImportSettings());

  static bool writeTableToCVSFile(vtkTable* table, const QString& fileName);

  static void fillExtendedTable(vtkTable* sourceTable, vtkExtendedTable* destTable,
                                const voDelimitedTextImportSettings& settings = voDelimitedTextImportSettings());

  void openCSVFile(const QString& fileName,const voDelimitedTextImportSettings& settings);
  void loadPhyloTreeDataSet(const QString& fileName,const QString& tableFileName,const voDelimitedTextImportSettings& settings);
  void loadPhyloTreeDataSet(const QString& fileName);

  static bool writeDataObjectToFile(vtkDataObject * dataObject, const QString& fileName);

  void createTreeHeatmapItem(QString name, voInputFileDataObject * treeObject,
                             voInputFileDataObject * tableObject);
  void createTreeHeatmapItem(QString name, QList<voInputFileDataObject *> forest,
                             voInputFileDataObject * tableObject);

  void saveState(const QString& fileName);
  void loadState(const QString& fileName);

protected:
  bool treeAndTableMatch(vtkTree *tree, vtkTable *table);
  void writeItemToXML(QStandardItem* parent, QXmlStreamWriter *stream);
  void writeTreeHeatmapToXML(voDataModelItem *item, QXmlStreamWriter *stream);
  void writeInputToXML(const QString& type, voDataModelItem *item,
                       QXmlStreamWriter *stream);
  void writeTableSettingsToXML(voDataModelItem *item, QXmlStreamWriter *stream);
  void loadTreeHeatmapFromXML(QXmlStreamReader *stream);
  void loadTreeFromXML(QXmlStreamReader *stream);
  void loadTableFromXML(QXmlStreamReader *stream);
  QString readTreeFileNameFromXML(QXmlStreamReader *stream);
  voDelimitedTextImportSettings readTableFromXML(QXmlStreamReader *stream,
                                                 QString *fileName);

  QMap<voInputFileDataObject *, voDelimitedTextImportSettings>
    tableSettings;
};

#endif
