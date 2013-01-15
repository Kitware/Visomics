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
#include <QDebug>
#include <QFileInfo>
#include <QStandardItem>
#include <QXmlStreamWriter>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisDriver.h"
#include "voApplication.h"
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voInputFileDataObject.h"
#include "voIOManager.h"
#include "voRegistry.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkDelimitedTextReader.h>
#include <vtkDelimitedTextWriter.h>
#include <vtkDoubleArray.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkMultiNewickTreeReader.h>
#include <vtkNewickTreeReader.h>
#include <vtkMultiPieceDataSet.h>
#include <vtkGraphLayoutView.h>
#include <vtkTree.h>


// --------------------------------------------------------------------------
bool voIOManager::readCSVFileIntoTable(const QString& fileName, vtkTable * outputTable, const voDelimitedTextImportSettings& settings, const bool haveHeaders)
{
  if (!outputTable)
    {
    return false;
    }

  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetFileName(fileName.toLatin1());

  // Configure reader
  reader->SetFieldDelimiterCharacters(
        settings.value(voDelimitedTextImportSettings::FieldDelimiterCharacters).toString().toLatin1());
  reader->SetMergeConsecutiveDelimiters(
        settings.value(voDelimitedTextImportSettings::MergeConsecutiveDelimiters).toBool());
  reader->SetStringDelimiter(
        settings.value(voDelimitedTextImportSettings::StringDelimiter).toChar().toLatin1());
  reader->SetUseStringDelimiter(
        settings.value(voDelimitedTextImportSettings::UseStringDelimiter).toBool());
  reader->SetHaveHeaders(haveHeaders);

  // Read data
  reader->Update();

  outputTable->ShallowCopy(reader->GetOutput());

  return true;
}

// --------------------------------------------------------------------------
bool voIOManager::readCSVFileIntoExtendedTable(const QString& fileName,
                                               vtkExtendedTable *outputTable,
                                               const voDelimitedTextImportSettings& settings)
{
  if (!outputTable)
    {
    return false;
    }

  vtkNew<vtkTable> dataTable;
  vtkNew<vtkTable> inputToExtendedTable;
  Self::readCSVFileIntoTable(fileName, dataTable.GetPointer(), settings, true);
  Self::readCSVFileIntoTable(fileName, inputToExtendedTable.GetPointer(), settings, false);
  Self::fillExtendedTable(inputToExtendedTable.GetPointer(), outputTable, settings);
  outputTable->SetInputDataTable(dataTable.GetPointer());

  return true;
}

// --------------------------------------------------------------------------
bool voIOManager::writeTableToCVSFile(vtkTable* table, const QString& fileName)
{
  if (!table)
    {
    return false;
    }

  vtkNew<vtkDelimitedTextWriter> writer;

  voDelimitedTextImportSettings settings;

  // Configure writer
  writer->SetFieldDelimiter(
        settings.value(voDelimitedTextImportSettings::FieldDelimiterCharacters).toString().toLatin1());
  writer->SetStringDelimiter(
        QString(settings.value(voDelimitedTextImportSettings::StringDelimiter).toChar()).toLatin1());
  writer->SetUseStringDelimiter(
        settings.value(voDelimitedTextImportSettings::UseStringDelimiter).toBool());

  writer->SetFileName(fileName.toLatin1());

  writer->SetInputData(table);
  writer->Update();

  return true;
}

// --------------------------------------------------------------------------
void voIOManager::fillExtendedTable(vtkTable* sourceTable, vtkExtendedTable* destTable,
                                    const voDelimitedTextImportSettings& settings)
{
  // vtkExtendedTable settings
  bool transpose = settings.value(voDelimitedTextImportSettings::Transpose).toBool();
  if (transpose)
    {
    voUtils::transposeTable(sourceTable);
    }

  int numberOfRowMetaDataTypes =
      settings.value(voDelimitedTextImportSettings::NumberOfRowMetaDataTypes).toInt();
  int numberOfColumnMetaDataTypes =
      settings.value(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes).toInt();

  Q_ASSERT(numberOfColumnMetaDataTypes <= sourceTable->GetNumberOfRows());

  //sourceTable->Dump();

  // ColumnMetaData
  vtkNew<vtkTable> columnMetaData;
  for (int cid = numberOfRowMetaDataTypes; cid < sourceTable->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(sourceTable->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = 0; rid < numberOfColumnMetaDataTypes; ++rid)
      {
      vtkSmartPointer<vtkStringArray> newColumn;
      if (cid == numberOfRowMetaDataTypes)
        {
        newColumn = vtkSmartPointer<vtkStringArray>::New();
        newColumn->SetNumberOfValues(sourceTable->GetNumberOfColumns() - numberOfRowMetaDataTypes);
        columnMetaData->AddColumn(newColumn);
        }
      else
        {
        newColumn = vtkStringArray::SafeDownCast(columnMetaData->GetColumn(rid));
        }
      Q_ASSERT(newColumn);
      vtkStdString value = column->GetValue(rid);
      newColumn->SetValue(cid - numberOfRowMetaDataTypes, value);
      }
    }

  //columnMetaData->Dump();

  // ColumnMetaDataLabels
  vtkNew<vtkStringArray> columnMetaDataLabels;
  if (numberOfRowMetaDataTypes > 0) // If there are no row metadata types, there is no room for column metadata labels
    {
    for (int rid = 0; rid < numberOfColumnMetaDataTypes; rid++)
      {
      columnMetaDataLabels->InsertNextValue(sourceTable->GetValue(rid, 0).ToString());
      }
    }

  // RowMetaData
  vtkNew<vtkTable> rowMetaData;
  Q_ASSERT(numberOfRowMetaDataTypes <= sourceTable->GetNumberOfColumns());
  for (int cid = 0; cid < numberOfRowMetaDataTypes; ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(sourceTable->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = numberOfColumnMetaDataTypes; rid < column->GetNumberOfValues(); ++rid)
      {
      vtkSmartPointer<vtkStringArray> newColumn;
      if (rid == numberOfColumnMetaDataTypes)
        {
        newColumn = vtkSmartPointer<vtkStringArray>::New();
        newColumn->SetNumberOfValues(sourceTable->GetNumberOfRows() - numberOfColumnMetaDataTypes);
        rowMetaData->AddColumn(newColumn);
        }
      else
        {
        newColumn = vtkStringArray::SafeDownCast(rowMetaData->GetColumn(cid));
        }
      Q_ASSERT(newColumn);
      vtkStdString value = column->GetValue(rid);
      newColumn->SetValue(rid - numberOfColumnMetaDataTypes, value);
      }
    }

  //rowMetaData->Dump();

  // RowMetaDataLabels
  vtkNew<vtkStringArray> rowMetaDataLabels;
  if (numberOfColumnMetaDataTypes > 0) // If there are no column metadata types, there is no room for row metadata labels
    {
    for (int cid = 0; cid < numberOfRowMetaDataTypes; cid++)
      {
      rowMetaDataLabels->InsertNextValue(sourceTable->GetValue(0, cid).ToString());
      }
    }

  // Data
  vtkNew<vtkTable> data;
  for (int cid = numberOfRowMetaDataTypes; cid < sourceTable->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(sourceTable->GetColumn(cid));
    Q_ASSERT(column);

    // determine whether this column contains numerical or categorical data.
    vtkNew<vtkDoubleArray> doubleColumn;
    vtkNew<vtkStringArray> stringColumn;
    vtkStdString value = column->GetValue(numberOfColumnMetaDataTypes);
    bool dataIsNumerical = false;
    vtkVariant(value).ToDouble(&dataIsNumerical);
    if (dataIsNumerical)
      {
      doubleColumn->SetNumberOfValues(
        sourceTable->GetNumberOfRows() - numberOfColumnMetaDataTypes);
      data->AddColumn(doubleColumn.GetPointer());
      }
    else
      {
      stringColumn->SetNumberOfValues(
        sourceTable->GetNumberOfRows() - numberOfColumnMetaDataTypes);
      data->AddColumn(stringColumn.GetPointer());
      }

    for (int rid = numberOfColumnMetaDataTypes; rid < column->GetNumberOfValues(); ++rid)
      {
      vtkStdString value = column->GetValue(rid);
      if (dataIsNumerical)
        {
        // Convert to numeric
        bool ok = false;
        double doubleValue = vtkVariant(value).ToDouble(&ok);
        if (!ok)
          {
          qCritical() << "Data at column" << cid << "and row" << rid << "is not a numeric value !"
                      << " - Defaulting to 0";
          doubleValue = 0;
          }
        doubleColumn->SetValue(rid - numberOfColumnMetaDataTypes, doubleValue);
        }
      else
        {
        stringColumn->SetValue(rid - numberOfColumnMetaDataTypes, value);
        }
      }
    }

  // ColumnMetaDataTypeOfInterest
  int columnMetaDataTypeOfInterest =
      settings.value(voDelimitedTextImportSettings::ColumnMetaDataTypeOfInterest).toInt();

  // RowMetaDataTypeOfInterest
  int rowMetaDataTypeOfInterest =
      settings.value(voDelimitedTextImportSettings::RowMetaDataTypeOfInterest).toInt();

  destTable->SetColumnMetaDataTable(columnMetaData.GetPointer());
  destTable->SetRowMetaDataTable(rowMetaData.GetPointer());
  destTable->SetData(data.GetPointer());
  destTable->SetColumnMetaDataTypeOfInterest(columnMetaDataTypeOfInterest);
  destTable->SetRowMetaDataTypeOfInterest(rowMetaDataTypeOfInterest);
  destTable->SetColumnMetaDataLabels(columnMetaDataLabels.GetPointer());
  destTable->SetRowMetaDataLabels(rowMetaDataLabels.GetPointer());

  // Set column names
  voUtils::setTableColumnNames(destTable->GetData(), destTable->GetColumnMetaDataOfInterestAsString());

  // NormalizationMethod
  QString normalizationMethod =
      settings.value(voDelimitedTextImportSettings::NormalizationMethod).toString();

  // Normalize
  if (voApplication::application())
    {
    voApplication::application()->normalizerRegistry()->apply(
          normalizationMethod, destTable->GetData(), settings);
    }
}

// --------------------------------------------------------------------------
void voIOManager::openCSVFile(const QString& fileName, const voDelimitedTextImportSettings& settings)
{
  vtkNew<vtkExtendedTable> extendedTable;
  Self::readCSVFileIntoExtendedTable(fileName, extendedTable.GetPointer(),
                                     settings);
  voInputFileDataObject * dataObject =
      new voInputFileDataObject(fileName, extendedTable.GetPointer());

  tableSettings.insert(dataObject, const_cast<voDelimitedTextImportSettings&>(settings));

  voDataModel * model = voApplication::application()->dataModel();
  voDataModelItem * newItem = model->addDataObject(dataObject);
  newItem->setRawViewType("voExtendedTableView");

  // Select added item
  model->setSelected(newItem);
}

// --------------------------------------------------------------------------
void voIOManager::loadPhyloTreeDataSet(const QString& fileName)
{
  // load the phylo tree data set file
  vtkSmartPointer<vtkMultiNewickTreeReader> reader =
        vtkSmartPointer<vtkMultiNewickTreeReader>::New();

  reader->SetFileName(fileName.toStdString().c_str());
  vtkMultiPieceDataSet * forest = reader->GetOutput();
  reader->Update();


  voDataModel * model = voApplication::application()->dataModel();

  if (forest->GetNumberOfPieces() == 1)
     { //single tree
     vtkTree * tree =  vtkTree::SafeDownCast( forest->GetPieceAsDataObject(0));
     voInputFileDataObject * dataObject =
       new voInputFileDataObject(fileName, tree);
     voDataModelItem * newItem = model->addDataObject(dataObject);
     newItem->setRawViewType("voTreeHeatmapView");

     model->setSelected(newItem);
     }
  else
     {//multiple trees --forest
     voDataObject * emptyDataObject = new voDataObject(QFileInfo(fileName).baseName(),NULL);
     voDataModelItem * newForestItem = model->addDataObject(emptyDataObject);
     for (unsigned int i = 0; i < forest->GetNumberOfPieces(); i++)
       {
       vtkTree * tree =  vtkTree::SafeDownCast( forest->GetPieceAsDataObject(i));
       QString displayName = QString("tree-%1").arg(QString::number(i));
       voInputFileDataObject * dataObject = new voInputFileDataObject(displayName, tree);
       voDataModelItem * treeItem = model->addDataObjectAsChild(dataObject, newForestItem);
       treeItem->setRawViewType("voTreeHeatmapView");

       newForestItem->addChildItem(treeItem);
       }
     model->setSelected(newForestItem);
     }
}

// --------------------------------------------------------------------------
void voIOManager::loadPhyloTreeDataSet(const QString& fileName,
  const QString & tableFileName, const voDelimitedTextImportSettings& settings )
{
  // load the phylo tree
  vtkSmartPointer<vtkMultiNewickTreeReader> reader =
        vtkSmartPointer<vtkMultiNewickTreeReader>::New();
  reader->SetFileName(fileName.toStdString().c_str());
  vtkMultiPieceDataSet * forest = reader->GetOutput();
  reader->Update();

  // load the associated table data
  vtkNew<vtkExtendedTable> extendedTable;
  Self::readCSVFileIntoExtendedTable(tableFileName, extendedTable.GetPointer(),
                                     settings);
  voInputFileDataObject * tableObject = new voInputFileDataObject(
    tableFileName, extendedTable.GetPointer());

  tableSettings.insert(tableObject,
                       const_cast<voDelimitedTextImportSettings&>(settings));

  QString treeHeatmapName =
    QString("%1 TreeHeatmap").arg(QFileInfo(fileName).baseName());

  //single tree
  if (forest->GetNumberOfPieces() == 1)
    {
    vtkTree * tree =  vtkTree::SafeDownCast( forest->GetPieceAsDataObject(0));

    if (!this->treeAndTableMatch(tree, extendedTable->GetInputData()))
      {
      return;
      }

    voInputFileDataObject * treeObject =
      new voInputFileDataObject(fileName, tree);

    this->createTreeHeatmapItem(treeHeatmapName, treeObject, tableObject);
    }

  // multiple trees
  else
    {
    QList<voInputFileDataObject *> treeObjects;
    for (unsigned int i = 0; i < forest->GetNumberOfPieces(); i++)
      {
      vtkTree * tree =  vtkTree::SafeDownCast( forest->GetPieceAsDataObject(i));
      if (!this->treeAndTableMatch(tree, extendedTable->GetInputData()))
        {
        return;
        }

      QString displayName = QString("tree-%1").arg(QString::number(i));
      voInputFileDataObject * treeObject = new voInputFileDataObject(displayName, tree);
      treeObjects << treeObject;
      }

    this->createTreeHeatmapItem(treeHeatmapName, treeObjects, tableObject);
    }
}

// --------------------------------------------------------------------------
bool voIOManager::treeAndTableMatch(vtkTree *tree, vtkTable *table)
{
  unsigned int NumberOfLeafNodes = 0;
  for (vtkIdType vertex = 0; vertex < tree->GetNumberOfVertices(); ++vertex)
    {
    if (!tree->IsLeaf(vertex))
      {
      continue;
      }
    ++NumberOfLeafNodes;
    }

  if (table->GetNumberOfRows() != NumberOfLeafNodes )
    {
    return false;
    }
  return true;
}

// --------------------------------------------------------------------------
void voIOManager::createTreeHeatmapItem(QString name,
                                        voInputFileDataObject * treeObject,
                                        voInputFileDataObject * tableObject)
{
  voDataModel * model = voApplication::application()->dataModel();

  // create a new item for our TreeHeatmap
  voDataObject * emptyDataObject = new voDataObject(name, NULL);
  voDataModelItem * newItem = model->addDataObject(emptyDataObject);

  // create a DataModelItem for our tree
  voDataModelItem * treeItem = model->addDataObjectAsChild(treeObject, newItem);
  treeItem->setRawViewType("voTreeHeatmapView");
  newItem->addChildItem(treeItem);

  // do the same for our table
  voDataModelItem * tableItem = model->addDataObjectAsChild(tableObject, newItem);
  tableItem->setType(voDataModelItem::InputType);
  tableItem->setRawViewType("voExtendedTableView");
  newItem->addChildItem(tableItem);

  // set up the visualization of the new TreeHeatmap
  newItem->setRawViewType("voTreeHeatmapView");
  newItem->setType(voDataModelItem::InputType);

  model->setSelected(newItem);
}

// --------------------------------------------------------------------------
void voIOManager::createTreeHeatmapItem(QString name,
                                        QList<voInputFileDataObject *> treeObjects,
                                        voInputFileDataObject * tableObject)
{
  voDataModel * model = voApplication::application()->dataModel();

  // create a new item for our TreeHeatmap
  voDataObject * emptyDataObject = new voDataObject(name, NULL);
  voDataModelItem * newItem = model->addDataObject(emptyDataObject);

  // create a DataModelItem for each of our trees
  for (int i = 0; i < treeObjects.size(); ++i)
    {
    voInputFileDataObject * treeObject = treeObjects[i];
    voDataModelItem * treeItem = model->addDataObjectAsChild(treeObject, newItem);
    treeItem->setRawViewType("voTreeHeatmapView");
    newItem->addChildItem(treeItem);
    }

  // do the same for our table
  voDataModelItem * tableItem = model->addDataObjectAsChild(tableObject, newItem);
  tableItem->setType(voDataModelItem::InputType);
  tableItem->setRawViewType("voExtendedTableView");
  newItem->addChildItem(tableItem);

  // set up the visualization of the new TreeHeatmap
  newItem->setRawViewType("voTreeHeatmapView");
  newItem->setType(voDataModelItem::InputType);

  model->setSelected(newItem);
}

// --------------------------------------------------------------------------
bool voIOManager::writeDataObjectToFile(vtkDataObject * dataObject, const QString& fileName)
{
  if (!dataObject)
    {
    return false;
    }

  vtkNew<vtkGenericDataObjectWriter> dataWriter;
  dataWriter->SetFileName(fileName.toLatin1());
  dataWriter->SetInputData(dataObject);
  dataWriter->Update();

  return true;
}

// --------------------------------------------------------------------------
void voIOManager::saveState(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qCritical() << "Could not open " << fileName << " for writing!";
    return;
  }

  QXmlStreamWriter stream(&file);
  stream.setAutoFormatting(true);
  stream.writeStartDocument();

  QStandardItem *parent = NULL;
  this->writeItemToXML(parent, &stream);

  stream.writeEndDocument();
}

// --------------------------------------------------------------------------
void voIOManager::writeItemToXML(QStandardItem* parent,
                                 QXmlStreamWriter *stream)
{
  voDataModel * dataModel = voApplication::application()->dataModel();
  voAnalysisDriver *driver = voApplication::application()->analysisDriver();

  if (!parent)
    {
    parent = dataModel->invisibleRootItem();
    }

  for (int row = 0; row < dataModel->rowCount(parent->index()); ++row)
    {
    for (int col = 0; col < dataModel->columnCount(parent->index()); ++col)
      {
      voDataModelItem *item =
        dynamic_cast<voDataModelItem*>(
        dataModel->itemFromIndex(dataModel->index(row, col, parent->index())));

      if (item->data(voDataModelItem::IsAnalysisContainerRole).toBool())
        {
        voAnalysis *analysis = reinterpret_cast<voAnalysis*>(
              item->data(voDataModelItem::AnalysisVoidStarRole).value<void*>());
        QString analysisName = analysis->objectName();
        //std::cout << " Analysis " << analysisName.toStdString() << " has input(s) ";
        for (int i = 0; i < driver->numberOfInputsForAnalysis(analysisName); ++i)
          {
          //std::cout << analysis->input(i)->name().toStdString() << ", ";
          }
        }
      else if (item->type() == voDataModelItem::InputType)
        {
        // Only record top-level input.  Child input objects are saved
        // with their parents.
        if (parent != dataModel->invisibleRootItem())
          {
          continue;
          }

        if (item->rawViewType() == "voTreeHeatmapView")
          {
          if (dataModel->hasChildren(item->index()))
            {
            this->writeTreeHeatmapToXML(item, stream);
            }
          else
            {
            this->writeInputToXML("Tree", item, stream);
            }
          }
        else if(item->rawViewType() == "voExtendedTableView" ||
                item->rawViewType() == "voTableView")
          {
          this->writeInputToXML("Table", item, stream);
          }
        }

      if (dataModel->hasChildren(item->index()))
        {
        this->writeItemToXML(item, stream);
        }
      }
    }
}

// --------------------------------------------------------------------------
void voIOManager::writeTreeHeatmapToXML(voDataModelItem *item,
                                        QXmlStreamWriter *stream)
{
  stream->writeStartElement("input");
  stream->writeAttribute("type", "TreeHeatmap");
  // Children are saved as rows (not columns)
  for (int i = 0; i < item->rowCount(); ++i)
    {
    voDataModelItem *childItem =
      dynamic_cast<voDataModelItem*>(item->child(i));
    voInputFileDataObject *inputObject =
      dynamic_cast<voInputFileDataObject*>(childItem->dataObject());
    if (!inputObject)
      {
      continue;
      }
    stream->writeStartElement("input");
    if(childItem->rawViewType() == "voExtendedTableView" ||
       childItem->rawViewType() == "voTableView")
      {
      stream->writeAttribute("type", "Table");
      this->writeTableSettingsToXML(childItem, stream);
      }
    else
      {
      stream->writeAttribute("type", "Tree");
      }
    stream->writeStartElement("filename");
    stream->writeCharacters(inputObject->fileName());
    stream->writeEndElement(); // filename
    stream->writeEndElement(); // tree or table input
    }
  stream->writeEndElement(); // tree heatmap input
}

// --------------------------------------------------------------------------
void voIOManager::writeInputToXML(const QString& type, voDataModelItem *item,
                                  QXmlStreamWriter *stream)
{
  stream->writeStartElement("input");
  stream->writeAttribute("type", type);
  voInputFileDataObject *inputObject =
    dynamic_cast<voInputFileDataObject*>(item->dataObject());
  if (!inputObject)
    {
    qCritical() << "Could not resolve " << item->rawViewType()
                << " as an input data file!";
    stream->writeEndElement(); // input
    return;
    }
  if (type == "Table")
    {
    this->writeTableSettingsToXML(item, stream);
    }
  stream->writeStartElement("filename");
  stream->writeCharacters(inputObject->fileName());
  stream->writeEndElement(); // filename
  stream->writeEndElement(); // input
}

// --------------------------------------------------------------------------
void voIOManager::writeTableSettingsToXML(voDataModelItem *item,
                                          QXmlStreamWriter *stream)
{
  voInputFileDataObject *tableObject =
    dynamic_cast<voInputFileDataObject*>(item->dataObject());
  if (!tableObject)
    {
    qCritical() << "Could not resolve " << item->text() << " as an input data file";
    return;
    }

  if (!this->tableSettings.contains(tableObject))
    {
    qCritical() << "No settings are stored for table " << item->text();
    return;
    }

  voDelimitedTextImportSettings settings =
    this->tableSettings.value(tableObject);

  stream->writeStartElement("table_reader_settings");

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "FieldDelimiterCharacters");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::FieldDelimiterCharacters).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "MergeConsecutiveDelimiters");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::MergeConsecutiveDelimiters).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "StringDelimiter");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::StringDelimiter).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "UseStringDelimiter");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::UseStringDelimiter).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "Transpose");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::Transpose).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "NumberOfColumnMetaDataTypes");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "ColumnMetaDataTypeOfInterest");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::ColumnMetaDataTypeOfInterest).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "NumberOfRowMetaDataTypes");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::NumberOfRowMetaDataTypes).toString());
  stream->writeEndElement();

  stream->writeStartElement("setting");
  stream->writeAttribute("name", "RowMetaDataTypeOfInterest");
  stream->writeCharacters(settings.value(
    voDelimitedTextImportSettings::RowMetaDataTypeOfInterest).toString());
  stream->writeEndElement();

  stream->writeEndElement(); // table_reader_settings
}

// --------------------------------------------------------------------------
void voIOManager::loadState(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qCritical() << "Could not open " << fileName << " for reading!";
    return;
  }
  QXmlStreamReader stream(&file);

  while (!stream.atEnd())
    {
    QString attribute = "";
    stream.readNext();
    if (stream.isStartElement())
      {
      QString name = stream.name().toString();
      if (name == "input")
        {
        QStringRef type = stream.attributes().value("type");
        if (type == "")
          {
          qCritical() << "empty type attribute encountered!";
          }
        else if (type == "TreeHeatmap")
          {
          this->loadTreeHeatmapFromXML(&stream);
          }
        else if (type == "Tree")
          {
          this->loadTreeFromXML(&stream);
          }
        else if (type == "Table")
          {
          this->loadTableFromXML(&stream);
          }
        }
      }
    }
  if (stream.hasError())
    {
    // do error handling
    }
}

// --------------------------------------------------------------------------
void voIOManager::loadTreeHeatmapFromXML(QXmlStreamReader *stream)
{
  stream->readNextStartElement();  // input (tree)
  QString name = stream->name().toString();
  if (name != "input")
    {
    qCritical() << "expected input, found " << name;
    return;
    }
 QString treeFile = this->readTreeFileNameFromXML(stream);
 if (treeFile == "")
   {
   qCritical() << "tree filename is empty string";
   return;
   }

  stream->readNext(); // </filename>
  stream->readNext(); // </input>

  stream->readNextStartElement(); // <input>
  name = stream->name().toString();
  if (name != "input")
    {
    qCritical() << "expected input, found " << name;
    return;
    }
  QStringRef type = stream->attributes().value("type");
  if (type != "Table")
    {
    qCritical() << "expected Table, found " << type;
    return;
    }

  QString tableFile;
  voDelimitedTextImportSettings settings =
    this->readTableFromXML(stream, &tableFile);
 if (tableFile == "")
   {
   qCritical() << "tree filename is empty string";
   return;
   }

  this->loadPhyloTreeDataSet(treeFile, tableFile, settings);
}

// --------------------------------------------------------------------------
void voIOManager::loadTreeFromXML(QXmlStreamReader *stream)
{
  QString fileName = this->readTreeFileNameFromXML(stream);
 if (fileName == "")
   {
   qCritical() << "tree filename is empty string";
   return;
   }
  this->loadPhyloTreeDataSet(fileName);
}

// --------------------------------------------------------------------------
void voIOManager::loadTableFromXML(QXmlStreamReader *stream)
{
  QString fileName;
  voDelimitedTextImportSettings settings =
    this->readTableFromXML(stream, &fileName);
 if (fileName == "")
   {
   qCritical() << "table filename is empty string";
   return;
   }
  this->openCSVFile(fileName, settings);
}

// --------------------------------------------------------------------------
QString voIOManager::readTreeFileNameFromXML(QXmlStreamReader *stream)
{
  QStringRef type = stream->attributes().value("type");
  if (type != "Tree")
    {
    qCritical() << "expected Tree, found " << type;
    return "";
    }

  stream->readNextStartElement();  // filename of Tree
  QString name = stream->name().toString();
  if (name != "filename")
    {
    qCritical() << "expecting filename, found " << name;
    return "";
    }

 return stream->readElementText();
}

// --------------------------------------------------------------------------
voDelimitedTextImportSettings voIOManager::readTableFromXML(
  QXmlStreamReader *stream, QString *fileName)
{
  voDelimitedTextImportSettings settings;
  stream->readNextStartElement();
  QString name = stream->name().toString();
  if (name != "table_reader_settings")
    {
    qCritical() << "expected table_reader_settings, found " << name;
    return settings;
    }

  // read table settings
  stream->readNext();
  stream->readNextStartElement();
  name = stream->name().toString();
  while (name != "table_reader_settings" && !stream->atEnd())
    {
    if (name != "setting")
      {
      qCritical() << "expected setting, found " << name;
      return settings;
      }

    QStringRef settingType = stream->attributes().value("name");
    QString value = stream->readElementText();
    if (settingType == "FieldDelimiterCharacters") // QString
      {
      settings.insert(voDelimitedTextImportSettings::FieldDelimiterCharacters,
                      value);
      }
    else if ("MergeConsecutiveDelimiters") // bool
      {
      bool b = false;
      if (value == "true")
        {
        b = true;
        }
      settings.insert(voDelimitedTextImportSettings::MergeConsecutiveDelimiters,
                      b);
      }
    else if ("StringDelimiter") // char
      {
      settings.insert(voDelimitedTextImportSettings::StringDelimiter,
                      value.at(0).toAscii());
      }
    else if ("UseStringDelimiter") // bool
      {
      bool b = false;
      if (value == "true")
        {
        b = true;
        }
      settings.insert(voDelimitedTextImportSettings::UseStringDelimiter,
                      b);
      }
    else if ("Transpose") // bool
      {
      bool b = false;
      if (value == "true")
        {
        b = true;
        }
      settings.insert(voDelimitedTextImportSettings::Transpose,
                      b);
      }
    else if ("NumberOfColumnMetaDataTypes") // int
      {
      settings.insert(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes,
                      value.toInt());
      }
    else if ("ColumnMetaDataTypeOfInterest") // int
      {
      settings.insert(voDelimitedTextImportSettings::ColumnMetaDataTypeOfInterest,
                      value.toInt());
      }
    else if ("NumberOfRowMetaDataTypes") // int
      {
      settings.insert(voDelimitedTextImportSettings::NumberOfRowMetaDataTypes,
                      value.toInt());
      }
    else if ("RowMetaDataTypeOfInterest") // int
      {
      settings.insert(voDelimitedTextImportSettings::RowMetaDataTypeOfInterest,
                      value.toInt());
      }
    else if ("NormalizationMethod") // QString
      {
      settings.insert(voDelimitedTextImportSettings::NormalizationMethod,
                      value);
      }
    else
      {
      qWarning() << "unhandled table setting encountered: " << settingType;
      }
    stream->readNextStartElement();
    name = stream->name().toString();
    }
  // end settings

  stream->readNextStartElement();
  name = stream->name().toString();
  if (name != "filename")
    {
    qCritical() << "expected filename, found " << name;
    return settings;
    }

 *fileName = stream->readElementText();
 return settings;
}
