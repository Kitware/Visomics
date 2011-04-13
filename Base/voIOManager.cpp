
// Qt includes
#include <QFileInfo>
#include <QDebug>

// Visomics includes
#include "voApplication.h"
#include "voCSVReader.h"
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voDataObject.h"
#include "voInputFileDataObject.h"
#include "voIOManager.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkDelimitedTextReader.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
void voIOManager::openCSVFiles(const QStringList& fileNames)
{
  QList<voDataObject*> loadedDataObjects;

  foreach(const QString& file, fileNames)
    {
    voCSVReader reader;
    reader.setFileName(file);
    reader.update();
    voInputFileDataObject * dataObject = new voInputFileDataObject();
    dataObject->setData(reader.output());
    dataObject->setName(QFileInfo(file).baseName());
    dataObject->setFileName(file);
    loadedDataObjects << dataObject;
    }

  voDataModel * model = voApplication::application()->dataModel();

  QList<voDataModelItem*> itemsAdded;
  foreach(voDataObject* dataObject, loadedDataObjects)
    {
    voDataModelItem * newItem = model->addDataObject(dataObject);
    newItem->setRawViewType("voTableView");
    itemsAdded << newItem;
    }

  // Select the first item added
  if (itemsAdded.count())
    {
    model->setSelected(itemsAdded.value(0));
    }
}

// --------------------------------------------------------------------------
void voIOManager::openCSVFile(const QString& fileName, const voDelimitedTextImportSettings& settings)
{
  settings.printAdditionalInfo();

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
  reader->SetHaveHeaders(
        settings.value(voDelimitedTextImportSettings::HaveHeaders).toBool());

  // Read data
  reader->Update();

  vtkTable* table = reader->GetOutput();

  // vtkExtendedTable settings
  bool transpose = settings.value(voDelimitedTextImportSettings::Transpose).toBool();
  if (transpose)
    {
    voUtils::transposeTable(table);
    }

  int numberOfRowMetaDataTypes =
      settings.value(voDelimitedTextImportSettings::NumberOfRowMetaDataTypes).toInt();
  int numberOfColumnMetaDataTypes =
      settings.value(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes).toInt();

  Q_ASSERT(numberOfColumnMetaDataTypes <= table->GetNumberOfRows());

  //table->Dump();

  // ColumnMetaData
  vtkNew<vtkTable> columnMetaData;
  for (int cid = numberOfRowMetaDataTypes; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(table->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = 0; rid < numberOfColumnMetaDataTypes; ++rid)
      {
      vtkSmartPointer<vtkStringArray> newColumn;
      if (cid == numberOfRowMetaDataTypes)
        {
        newColumn = vtkSmartPointer<vtkStringArray>::New();
        newColumn->SetNumberOfValues(table->GetNumberOfColumns() - numberOfRowMetaDataTypes);
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

  // RowMetaData
  vtkNew<vtkTable> rowMetaData;
  Q_ASSERT(numberOfRowMetaDataTypes <= table->GetNumberOfColumns());
  for (int cid = 0; cid < numberOfRowMetaDataTypes; ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(table->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = numberOfColumnMetaDataTypes; rid < column->GetNumberOfValues(); ++rid)
      {
      vtkSmartPointer<vtkStringArray> newColumn;
      if (rid == numberOfColumnMetaDataTypes)
        {
        newColumn = vtkSmartPointer<vtkStringArray>::New();
        newColumn->SetNumberOfValues(table->GetNumberOfRows() - numberOfColumnMetaDataTypes);
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

  // Data
  vtkNew<vtkTable> data;
  for (int cid = numberOfRowMetaDataTypes; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(table->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = numberOfColumnMetaDataTypes; rid < column->GetNumberOfValues(); ++rid)
      {
      vtkSmartPointer<vtkDoubleArray> newColumn;
      if (rid == numberOfColumnMetaDataTypes)
        {
        newColumn = vtkSmartPointer<vtkDoubleArray>::New();
        newColumn->SetNumberOfValues(table->GetNumberOfRows() - numberOfColumnMetaDataTypes);
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
        qCritical() << "Problem loading file" << fileName
                    << " - data at column" << cid << "and row" << rid << "is not a numeric value !"
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

  vtkNew<vtkExtendedTable> extendedTable;
  extendedTable->SetColumnMetaDataTable(columnMetaData.GetPointer());
  extendedTable->SetRowMetaDataTable(rowMetaData.GetPointer());
  extendedTable->SetData(data.GetPointer());
  extendedTable->SetColumnMetaDataTypeOfInterest(columnMetaDataTypeOfInterest);
  extendedTable->SetRowMetaDataTypeOfInterest(rowMetaDataTypeOfInterest);

  // Set column names
  voUtils::setTableColumnNames(extendedTable->GetData(), extendedTable->GetColumnMetaDataOfInterestAsString());

  voInputFileDataObject * dataObject = new voInputFileDataObject();
  dataObject->setData(extendedTable.GetPointer());
  dataObject->setName(QFileInfo(fileName).baseName());
  dataObject->setFileName(fileName);

  voDataModel * model = voApplication::application()->dataModel();

  voDataModelItem * newItem = model->addDataObject(dataObject);
  newItem->setRawViewType("voExtendedTableView");

  // Select added item
  model->setSelected(newItem);

  //extendedTable->Dump();
}
