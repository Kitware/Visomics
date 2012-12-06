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
#include <QFileInfo>
#include <QDebug>

// Visomics includes
#include "voAnalysis.h"
#include "voApplication.h"
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voInputFileDataObject.h"
#include "voIOManager.h"
#include "voRegistry.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"
#include "voTableDataObject.h"

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
    for (int rid = numberOfColumnMetaDataTypes; rid < column->GetNumberOfValues(); ++rid)
      {
      vtkSmartPointer<vtkDoubleArray> newColumn;
      if (rid == numberOfColumnMetaDataTypes)
        {
        newColumn = vtkSmartPointer<vtkDoubleArray>::New();
        newColumn->SetNumberOfValues(sourceTable->GetNumberOfRows() - numberOfColumnMetaDataTypes);
        data->AddColumn(newColumn);
        }
      else
        {
        newColumn = vtkDoubleArray::SafeDownCast(data->GetColumn(cid - numberOfRowMetaDataTypes));
        }
      Q_ASSERT(newColumn);
      vtkStdString value = column->GetValue(rid);
      // Convert to numeric
      bool ok = false;
      double doubleValue = vtkVariant(value).ToDouble(&ok);
      if (!ok)
        {
        qCritical() << "Data at column" << cid << "and row" << rid << "is not a numeric value !"
                    << " - Defaulting to 0";
        doubleValue = 0;
        }
      newColumn->SetValue(rid - numberOfColumnMetaDataTypes, doubleValue);
      }
    }

  //data->Dump();

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

  //destTable->GetData()->Dump();
}

// --------------------------------------------------------------------------
void voIOManager::openCSVFile(const QString& fileName, const voDelimitedTextImportSettings& settings)
{
  // settings.printAdditionalInfo();

  vtkNew<vtkTable> table;
  Self::readCSVFileIntoTable(fileName, table.GetPointer(), settings);

  vtkNew<vtkExtendedTable> extendedTable;
  Self::fillExtendedTable(table.GetPointer(), extendedTable.GetPointer(), settings);

  voInputFileDataObject * dataObject =
      new voInputFileDataObject(fileName, extendedTable.GetPointer());

  voDataModel * model = voApplication::application()->dataModel();

  voDataModelItem * newItem = model->addDataObject(dataObject);
  newItem->setRawViewType("voExtendedTableView");

  // Select added item
  model->setSelected(newItem);

  //extendedTable->Dump();
}

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
  voDataModel * model = voApplication::application()->dataModel();
  voDataObject * emptyDataObject = new voDataObject(QFileInfo(fileName).baseName(),NULL);
  voDataModelItem * newItem = model->addDataObject(emptyDataObject);

  // load the phylo tree
  vtkSmartPointer<vtkMultiNewickTreeReader> reader =
        vtkSmartPointer<vtkMultiNewickTreeReader>::New();

  reader->SetFileName(fileName.toStdString().c_str());
  vtkMultiPieceDataSet * forest = reader->GetOutput();
  reader->Update();

  // load associated table data
  vtkNew<vtkTable> table;
  Self::readCSVFileIntoTable(tableFileName, table.GetPointer(), settings, true);
  vtkNew<vtkExtendedTable> extendedTable;
  Self::fillExtendedTable(table.GetPointer(), extendedTable.GetPointer(), settings);
  extendedTable->SetInputDataTable(table.GetPointer());

  voTableDataObject * dataObjectTable =
    new voTableDataObject(QFileInfo(tableFileName).baseName(), extendedTable.GetPointer());

  if (forest->GetNumberOfPieces() == 1)
    { //single tree
    vtkTree * tree =  vtkTree::SafeDownCast( forest->GetPieceAsDataObject(0));
    voInputFileDataObject * dataObject =
      new voInputFileDataObject(fileName, tree);
    voDataModelItem * treeItem = model->addDataObjectAsChild(dataObject, newItem);
    treeItem->setRawViewType("voTreeHeatmapView");

    voDataModelItem * tableItem = model->addDataObjectAsChild(dataObjectTable,newItem);
    tableItem->setType(voDataModelItem::InputType);
    tableItem->setRawViewType("voTableView");

    // add table data
    // use tree heat map view, the tree and the table must be matching
    unsigned int NumberOfLeafNodes = 0;
    for (vtkIdType vertex = 0; vertex < tree->GetNumberOfVertices(); ++vertex)
      {
      if (!tree->IsLeaf(vertex))
        {
        continue;
        }
      ++NumberOfLeafNodes;
      }

    if (table->GetNumberOfRows() == NumberOfLeafNodes )
      {//display data table as a heat map
      newItem->setRawViewType("voTreeHeatmapView");
      newItem->setType(voDataModelItem::InputType);
      }
    newItem->addChildItem(treeItem);
    newItem->addChildItem(tableItem);

    }
  else
    {//multiple trees --forest
    for (unsigned int i = 0; i < forest->GetNumberOfPieces(); i++)
      {
      vtkTree * tree =  vtkTree::SafeDownCast( forest->GetPieceAsDataObject(i));
      QString displayName = QString("tree-%1").arg(QString::number(i));
      voInputFileDataObject * dataObject = new voInputFileDataObject(displayName, tree);
      voDataModelItem * treeItem = model->addDataObjectAsChild(dataObject, newItem);
      treeItem->setRawViewType("voTreeHeatmapView");

      newItem->addChildItem(treeItem);
      }

    //add table item
    voDataModelItem * tableItem = model->addDataObjectAsChild(dataObjectTable,newItem);
    tableItem->setRawViewType("voTableView");
    newItem->addChildItem(tableItem);
    }

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
