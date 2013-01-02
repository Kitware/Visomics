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

// QT includes
#include <QDebug>
#include <QFile>
#include <QPalette>
#include <QStringList>
#include <QTemporaryFile>
#include <QTextStream>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkDelimitedTextReader.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkStringToNumeric.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

// Visomics includes
#include "voDelimitedTextPreviewModel.h"
#include "voDelimitedTextImportSettings.h"
#include "voUtils.h"

class voDelimitedTextPreviewModelPrivate
{
  Q_DECLARE_PUBLIC(voDelimitedTextPreviewModel);

public:
  typedef voDelimitedTextPreviewModelPrivate Self;
  voDelimitedTextPreviewModelPrivate(const voDelimitedTextImportSettings& defaultSettings,
                                     voDelimitedTextPreviewModel& object);

  void loadFile();

  void configureReader(vtkDelimitedTextReader * reader);

  void updateDataPreview();

  static void updateDataPreviewCallback(vtkObject *caller, unsigned long eid,
                                        void *clientData, void * callData);
  vtkSmartPointer<vtkCallbackCommand> UpdateDataPreviewCallbackCommand;

  QTemporaryFile SampleCacheFile;

  QString FileName;
  char FieldDelimiter;
  char StringDelimiter; // Value of 0 indicates none
  bool MergeConsecutiveDelimiters;
  bool Transpose;

  int ColumnMetaDataTypeOfInterest; // From 0..(N-1) with N = NumberOfColumnMetaDataTypes
  int NumberOfColumnMetaDataTypes;

  int RowMetaDataTypeOfInterest; // From 0..(N-1) with N = NumberOfRowMetaDataTypes
  int NumberOfRowMetaDataTypes;

  int NumberOfRowsToPreview;

  bool InlineUpdate;

  vtkSmartPointer<vtkTable> DataTable;
  vtkSmartPointer<vtkTable> OriginalDataTable;

private:
  voDelimitedTextPreviewModel* const q_ptr;
};

// --------------------------------------------------------------------------
// voDelimitedTextPreviewModelPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewModelPrivate::voDelimitedTextPreviewModelPrivate(
    const voDelimitedTextImportSettings& defaultSettings, voDelimitedTextPreviewModel& object)
  : q_ptr(&object)
{
  this->FieldDelimiter = defaultSettings.value(voDelimitedTextImportSettings::FieldDelimiterCharacters).toString().at(0).toLatin1();
  this->StringDelimiter = defaultSettings.value(voDelimitedTextImportSettings::StringDelimiter).toChar().toLatin1();
  this->MergeConsecutiveDelimiters = defaultSettings.value(voDelimitedTextImportSettings::MergeConsecutiveDelimiters).toBool();
  this->Transpose = defaultSettings.value(voDelimitedTextImportSettings::Transpose).toBool();

  this->ColumnMetaDataTypeOfInterest = defaultSettings.value(voDelimitedTextImportSettings::ColumnMetaDataTypeOfInterest).toInt();
  this->NumberOfColumnMetaDataTypes = defaultSettings.value(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes).toInt();

  this->RowMetaDataTypeOfInterest = defaultSettings.value(voDelimitedTextImportSettings::RowMetaDataTypeOfInterest).toInt();
  this->NumberOfRowMetaDataTypes = defaultSettings.value(voDelimitedTextImportSettings::NumberOfRowMetaDataTypes).toInt();

  this->NumberOfRowsToPreview = 100;
  this->InlineUpdate = true;

  // If init() fails, SampleCacheFile will stay closed
  if (this->SampleCacheFile.isOpen())
    {
    this->SampleCacheFile.close();
    }

  this->DataTable = vtkSmartPointer<vtkTable>::New();
  this->OriginalDataTable = vtkSmartPointer<vtkTable>::New();
  this->UpdateDataPreviewCallbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  this->UpdateDataPreviewCallbackCommand->SetClientData(reinterpret_cast<void*>(this));
  this->UpdateDataPreviewCallbackCommand->SetCallback(Self::updateDataPreviewCallback);
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModelPrivate::loadFile()
{
  Q_ASSERT(QFile::exists(this->FileName));
  QFile infile(this->FileName);
  bool openStatus = infile.open(QIODevice::ReadOnly);
  if (!openStatus)
    {
    qWarning() << QObject::tr("File ") << this->FileName << QObject::tr(" could not be opened for reading.  Did something change between when you selected the file and now?");
    return;
    }

  // Read file
  QTextStream instream(&infile);
  QStringList sampleLinesList;
  for (int i = 0; i < this->NumberOfRowsToPreview && !instream.atEnd(); i++)
    {
    sampleLinesList << instream.readLine();
    }

  // Push lines to temp file
  QString sampleLines;
  sampleLines = sampleLinesList.join("\n");
  bool status = this->SampleCacheFile.open();
  if (!status)
    {
    qWarning() << QObject::tr("Couldn't open temporary file for preview!");
    return;
    }
  this->SampleCacheFile.write(sampleLines.toAscii());

  this->SampleCacheFile.close();
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModelPrivate::configureReader(vtkDelimitedTextReader * reader)
{
  Q_ASSERT(reader);

  reader->DetectNumericColumnsOff();
  reader->SetFileName(this->SampleCacheFile.fileName().toAscii().data());

  char delim_string[2];
  delim_string[0] = this->FieldDelimiter;
  delim_string[1] = '\0';
  //vtkUnicodeString delimiters = vtkUnicodeString::from_utf8(delim_string);
  //reader->SetUnicodeFieldDelimiters(delimiters);
  reader->SetFieldDelimiterCharacters(delim_string);

  this->MergeConsecutiveDelimiters = (this->FieldDelimiter == ' ' || this->FieldDelimiter == '\t');
  reader->SetMergeConsecutiveDelimiters(this->MergeConsecutiveDelimiters);

  if (this->StringDelimiter) // Value of 0 indicates none
    {
    reader->SetStringDelimiter(this->StringDelimiter);
    reader->SetUseStringDelimiter(true);
    }
  else
    {
    reader->SetUseStringDelimiter(false);
    }

  reader->SetHaveHeaders(false);
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModelPrivate::updateDataPreview()
{
  Q_ASSERT(this->DataTable.GetPointer());
  Q_Q(voDelimitedTextPreviewModel);

  for (vtkIdType cid = 0; cid < this->DataTable->GetNumberOfColumns(); ++cid)
    {
    vtkAbstractArray * column = this->DataTable->GetColumn(cid);
    for (int rid = 0; rid < column->GetNumberOfComponents() * column->GetNumberOfTuples(); ++rid)
      {
      QString value;
      if (vtkDoubleArray * doubleColumn = vtkDoubleArray::SafeDownCast(column))
        {
        value = QString::number(doubleColumn->GetValue(rid));
        }
      if (vtkIntArray * intColumn = vtkIntArray::SafeDownCast(column))
        {
        value = QString::number(intColumn->GetValue(rid));
        }
      else if (vtkStringArray * stringColumn = vtkStringArray::SafeDownCast(column))
        {
        value = QString::fromStdString(stringColumn->GetValue(rid));
        }
      Q_ASSERT(!value.isNull());
      QStandardItem * currentItem = new QStandardItem(value);
      q->setItem(rid + this->NumberOfColumnMetaDataTypes,
                 cid + this->NumberOfRowMetaDataTypes, currentItem);
      currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      }
    }
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModelPrivate::updateDataPreviewCallback(vtkObject *caller,
                                                                   unsigned long eid,
                                                                   void *clientData,
                                                                   void * callData)
{
  Q_UNUSED(callData);
  Q_UNUSED(caller);
  Q_UNUSED(eid);
  Q_ASSERT(eid == vtkCommand::ModifiedEvent);
  Q_ASSERT(clientData);
  voDelimitedTextPreviewModelPrivate * d =
        reinterpret_cast<voDelimitedTextPreviewModelPrivate*>(clientData);
  Q_ASSERT(d->DataTable.GetPointer() == caller);
  d->updateDataPreview();
}

// --------------------------------------------------------------------------
// voDelimitedTextPreviewModel methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewModel::voDelimitedTextPreviewModel(
    const voDelimitedTextImportSettings& defaultSettings, QObject* newParent) :
  Superclass(newParent), d_ptr(new voDelimitedTextPreviewModelPrivate(defaultSettings, *this))
{
  //Q_D(voDelimitedTextPreviewModel);
}

// --------------------------------------------------------------------------
voDelimitedTextPreviewModel::~voDelimitedTextPreviewModel()
{
}

// --------------------------------------------------------------------------
QString voDelimitedTextPreviewModel::fileName() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->FileName;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setFileName(const QString& newFileName)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->FileName == newFileName)
    {
    return;
    }

  if (!QFile::exists(newFileName))
    {
    return;
    }

  d->FileName = newFileName;

  d->loadFile();
  this->updatePreview();

  emit this->fileNameChanged(newFileName);
}

// --------------------------------------------------------------------------
QString voDelimitedTextPreviewModel::fieldDelimiterCharacters() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return QString(d->FieldDelimiter);
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setFieldDelimiter(char delimiter)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->FieldDelimiter == delimiter)
    {
    return;
    }
  d->FieldDelimiter = delimiter;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->fieldDelimiterChanged(delimiter);
}

// --------------------------------------------------------------------------
char voDelimitedTextPreviewModel::stringDelimiter() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->StringDelimiter;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setStringDelimiter(char character)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->StringDelimiter == character)
    {
    return;
    }
  d->StringDelimiter = character;
  if (d->InlineUpdate)
    {
    this->updatePreview();
    }
  emit this->stringDelimiterChanged(character);
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::transpose() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->Transpose;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setTranspose(bool value)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->Transpose == value)
    {
    return;
    }
  d->Transpose = value;

  // Switch row and column metdata type numbers
  int currentNumberOfColumnMetaDataTypes = d->NumberOfColumnMetaDataTypes;
  d->NumberOfColumnMetaDataTypes = d->NumberOfRowMetaDataTypes;
  d->NumberOfRowMetaDataTypes = currentNumberOfColumnMetaDataTypes;

  // Switch row and column metdata types of interest
  int currentColumnMetaDataTypeOfInterest = d->ColumnMetaDataTypeOfInterest;
  d->ColumnMetaDataTypeOfInterest = d->RowMetaDataTypeOfInterest;
  d->RowMetaDataTypeOfInterest = currentColumnMetaDataTypeOfInterest;

  // Do not call 'setNumberOfColumnMetaDataTypes()' to avoid extra call to 'updatePreview()'
  if (currentNumberOfColumnMetaDataTypes != d->NumberOfColumnMetaDataTypes)
    {
    // If column is changed, row is changed too (since they were swapped)
    emit this->numberOfColumnMetaDataTypesChanged(d->NumberOfColumnMetaDataTypes);
    emit this->numberOfRowMetaDataTypesChanged(d->NumberOfRowMetaDataTypes);
    }

  // Do not call 'setColumnMetaDataTypeOfInterest()' to avoid extra call to 'updatePreview()'
  if (currentColumnMetaDataTypeOfInterest != d->ColumnMetaDataTypeOfInterest)
    {
    emit this->columnMetaDataTypeOfInterestChanged(d->ColumnMetaDataTypeOfInterest);
    emit this->rowMetaDataTypeOfInterestChanged(d->RowMetaDataTypeOfInterest);
    }

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->transposeChanged(value);
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfColumnMetaDataTypes() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfColumnMetaDataTypes;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfColumnMetaDataTypes(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfColumnMetaDataTypes == count)
    {
    return;
    }
  d->NumberOfColumnMetaDataTypes = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->numberOfColumnMetaDataTypesChanged(d->NumberOfColumnMetaDataTypes);

  // Constrain ColumnMetaDataTypeOfInterest
  if (count < this->columnMetaDataTypeOfInterest())
    {
    this->setColumnMetaDataTypeOfInterest(count);
    }
  if (count == 0)
    {
    this->setColumnMetaDataTypeOfInterest(-1);
    }
  else if (count > 0 && this->columnMetaDataTypeOfInterest() == -1)
    {
    this->setColumnMetaDataTypeOfInterest(0);
    }
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::columnMetaDataTypeOfInterest() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->ColumnMetaDataTypeOfInterest;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setColumnMetaDataTypeOfInterest(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->ColumnMetaDataTypeOfInterest == count)
    {
    return;
    }
  d->ColumnMetaDataTypeOfInterest = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->columnMetaDataTypeOfInterestChanged(d->ColumnMetaDataTypeOfInterest);
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfRowMetaDataTypes() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfRowMetaDataTypes;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfRowMetaDataTypes(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfRowMetaDataTypes == count)
    {
    return;
    }
  d->NumberOfRowMetaDataTypes = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->numberOfRowMetaDataTypesChanged(d->NumberOfRowMetaDataTypes);

  // Constrain RowMetaDataTypeOfInterest
  if(count < this->rowMetaDataTypeOfInterest())
    {
    this->setRowMetaDataTypeOfInterest(count);
    }
  if (count == 0)
    {
    this->setRowMetaDataTypeOfInterest(-1);
    }
  else if (count > 0 && this->rowMetaDataTypeOfInterest() == -1)
    {
    this->setRowMetaDataTypeOfInterest(0);
    }
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::rowMetaDataTypeOfInterest() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->RowMetaDataTypeOfInterest;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setRowMetaDataTypeOfInterest(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->RowMetaDataTypeOfInterest == count)
    {
    return;
    }
  d->RowMetaDataTypeOfInterest = count;

  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->rowMetaDataTypeOfInterestChanged(d->RowMetaDataTypeOfInterest);
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfRowsToPreview() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfRowsToPreview;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfRowsToPreview(int count)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfRowsToPreview == count)
    {
    return;
    }
  d->NumberOfRowsToPreview = count;

  if (d->FileName.isEmpty())
    {
    return;
    }
  d->loadFile();
  this->updatePreview();

  emit this->numberOfRowsToPreviewChanged(count);
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::inlineUpdate() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->InlineUpdate;
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::mergeConsecutiveDelimiters()const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->MergeConsecutiveDelimiters;
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::useStringDelimiter()const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->StringDelimiter; // Value of 0 indicates None
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setInlineUpdate(bool value)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->InlineUpdate != value)
    {
    return;
    }
  d->InlineUpdate = value;
  if (d->InlineUpdate)
    {
    this->updatePreview();
    }

  emit this->inlineUpdateChanged(value);
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::resetDataTable()
{
  Q_D(voDelimitedTextPreviewModel);
  d->DataTable->DeepCopy(d->OriginalDataTable);
}

// --------------------------------------------------------------------------
vtkTable * voDelimitedTextPreviewModel::dataTable()
{
  Q_D(voDelimitedTextPreviewModel);
  return d->DataTable;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::updatePreview()
{
  Q_D(voDelimitedTextPreviewModel);

  if (d->SampleCacheFile.fileName().isEmpty())
    {
    //qWarning() << QObject::tr("ERROR: updatePreview: SampleCacheFile filename not set");
    return;
    }

  // Setup vtkDelimitedTextReader
  vtkNew<vtkDelimitedTextReader> previewReader;
  d->configureReader(previewReader.GetPointer());

  // Read in file to table
  previewReader->Update();
  vtkSmartPointer<vtkTable> table = previewReader->GetOutput();

  if (d->Transpose)
    {
    // Assumes there is a header column ... which we have no setting to specify for anyway
    voUtils::transposeTable(table);
    }

  // Build model (self)
  this->clear();

  // Clear original data table
  while (d->OriginalDataTable->GetNumberOfColumns() > 0)
    {
    d->OriginalDataTable->RemoveColumn(d->OriginalDataTable->GetNumberOfColumns() - 1); // columns should be named
    }

  // Update metadata preview and set DataTable
  QColor headerBackgroundColor = QPalette().color(QPalette::Window);
  QColor ofInterestBackgroundColor = QPalette().color(QPalette::Mid);

  for (vtkIdType cid = 0; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(table->GetColumn(cid));
    Q_ASSERT(column);
    vtkSmartPointer<vtkStringArray> dataColumn;
    if (cid >= d->NumberOfRowMetaDataTypes)
      {
      dataColumn = vtkSmartPointer<vtkStringArray>::New();
      dataColumn->SetName(QString::number(cid - d->NumberOfRowMetaDataTypes + 1).toLatin1());
      dataColumn->SetNumberOfValues(column->GetNumberOfValues() - d->NumberOfColumnMetaDataTypes);
      }
    for (int rid = 0; rid < column->GetNumberOfValues(); ++rid)
      {
      QString value = QString(column->GetValue(rid));
      QStandardItem* currentItem = 0;
      if (rid == d->ColumnMetaDataTypeOfInterest || cid == d->RowMetaDataTypeOfInterest)
        {
        this->setItem(rid, cid, (currentItem = new QStandardItem(value)));
        currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        currentItem->setData(ofInterestBackgroundColor, Qt::BackgroundRole);
        }
      else if (rid < d->NumberOfColumnMetaDataTypes || cid < d->NumberOfRowMetaDataTypes)
        {
        this->setItem(rid, cid, (currentItem = new QStandardItem(value)));
        currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        currentItem->setData(headerBackgroundColor, Qt::BackgroundRole);
        }
      else
        {
        dataColumn->SetValue(rid - d->NumberOfColumnMetaDataTypes, value.toLatin1());
        }
      }
    if (cid >= d->NumberOfRowMetaDataTypes)
      {
      d->OriginalDataTable->AddColumn(dataColumn.GetPointer());
      }
    }

  // TODO: Add missing value identification/rectification step
  vtkNew<vtkStringToNumeric> numericToStringFilter;
  numericToStringFilter->SetInputData(d->OriginalDataTable.GetPointer());
  numericToStringFilter->Update();
  vtkTable * numericDataTable = vtkTable::SafeDownCast(numericToStringFilter->GetOutput());
  Q_ASSERT(numericDataTable);
  d->OriginalDataTable = numericDataTable;

  // Remove data table observer, copy data, and re-add observer
  d->DataTable->RemoveObserver(d->UpdateDataPreviewCallbackCommand);
  this->resetDataTable();
  d->DataTable->AddObserver(vtkCommand::ModifiedEvent, d->UpdateDataPreviewCallbackCommand);

  d->updateDataPreview();
}
