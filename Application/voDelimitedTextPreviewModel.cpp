
//QT includes
#include <QDebug>
#include <QFile>
#include <QPalette>
#include <QStringList>
#include <QTemporaryFile>
#include <QTextStream>

//VTK includes
#include <vtkDelimitedTextReader.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

// Visomics includes
#include "voDelimitedTextPreviewModel.h"


class voDelimitedTextPreviewModelPrivate
{
  Q_DECLARE_PUBLIC(voDelimitedTextPreviewModel);

public:
  voDelimitedTextPreviewModelPrivate(voDelimitedTextPreviewModel& object);

  void loadFile();

  void configureReader(vtkDelimitedTextReader * reader);

  static void transposeTable(vtkTable* table);

  QTemporaryFile SampleCacheFile;

  QString FileName;
  char FieldDelimiter;
  char StringBeginEndCharacter; // Value of 0 indicates none
  bool UseFirstLineAsAttributeNames;
  bool Transpose;

  int ColumnMetaDataTypeOfInterest; // From 0..(N-1) with N = NumberOfColumnMetaDataTypes
  int NumberOfColumnMetaDataTypes;

  int RowMetaDataTypeOfInterest; // From 0..(N-1) with N = NumberOfRowMetaDataTypes
  int NumberOfRowMetaDataTypes;

  int NumberOfRowsToPreview;

  bool InlineUpdate;

private:
  voDelimitedTextPreviewModel* const q_ptr;
};

// --------------------------------------------------------------------------
// voDelimitedTextPreviewModelPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewModelPrivate::voDelimitedTextPreviewModelPrivate(voDelimitedTextPreviewModel& object)
  : q_ptr(&object)
{
  this->FieldDelimiter = ',';
  this->StringBeginEndCharacter = '\"';
  this->UseFirstLineAsAttributeNames = false;
  this->Transpose = false;

  this->ColumnMetaDataTypeOfInterest = 0;
  this->NumberOfColumnMetaDataTypes = 1;

  this->RowMetaDataTypeOfInterest = 0;
  this->NumberOfRowMetaDataTypes = 1;

  this->NumberOfRowsToPreview = 100;
  this->InlineUpdate = false;

  // If init() fails, SampleCacheFile will stay closed
  if (this->SampleCacheFile.isOpen())
    {
    this->SampleCacheFile.close();
    }
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
  delim_string[1] = 0;
  vtkUnicodeString delimiters = vtkUnicodeString::from_utf8(delim_string);
  reader->SetUnicodeFieldDelimiters(delimiters);

  if (this->FieldDelimiter == ' ' || this->FieldDelimiter == '\t')
    {
    reader->SetMergeConsecutiveDelimiters(true);
    }
  else
    {
    reader->SetMergeConsecutiveDelimiters(false);
    }

  if (this->StringBeginEndCharacter) // Value of 0 indicates none
    {
    reader->SetStringDelimiter(this->StringBeginEndCharacter);
    reader->SetUseStringDelimiter(true);
    }
  else
    {
    reader->SetUseStringDelimiter(false);
    }

  reader->SetHaveHeaders(this->UseFirstLineAsAttributeNames);
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModelPrivate::transposeTable(vtkTable* table)
{
  Q_ASSERT(table);

  vtkNew<vtkTable> transposeTable;

  vtkNew<vtkStringArray> header;
  header->SetName("header");
  header->SetNumberOfTuples(table->GetNumberOfColumns()-1);
  for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
    {
    header->SetValue(c-1, table->GetColumnName(c));
    }
  transposeTable->AddColumn(header.GetPointer());
  for (vtkIdType r = 0; r < table->GetNumberOfRows(); ++r)
    {
    vtkNew<vtkStringArray> newcol;
    newcol->SetName(table->GetValue(r, 0).ToString().c_str());
    newcol->SetNumberOfTuples(table->GetNumberOfColumns() - 1);
    for (vtkIdType c = 1; c < table->GetNumberOfColumns(); ++c)
      {
      newcol->SetValue(c-1, table->GetValue(r, c).ToString());
      }
    transposeTable->AddColumn(newcol.GetPointer());
    }
  table->DeepCopy(transposeTable.GetPointer());
}

// --------------------------------------------------------------------------
// voDelimitedTextPreviewModel methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewModel::voDelimitedTextPreviewModel(QObject* newParent) :
  Superclass(newParent), d_ptr(new voDelimitedTextPreviewModelPrivate(*this))
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
}

// --------------------------------------------------------------------------
char voDelimitedTextPreviewModel::fieldDelimiter() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->FieldDelimiter;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setFieldDelimiter (char _arg)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->FieldDelimiter != _arg)
    {
    d->FieldDelimiter = _arg;
    if (d->InlineUpdate)
      {
      this->updatePreview();
      }
    }
}

// --------------------------------------------------------------------------
char voDelimitedTextPreviewModel::stringBeginEndCharacter() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->StringBeginEndCharacter;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setStringBeginEndCharacter(char _arg)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->StringBeginEndCharacter != _arg)
    {
    d->StringBeginEndCharacter = _arg;
    if (d->InlineUpdate)
      {
      this->updatePreview();
      }
    }
}

//// --------------------------------------------------------------------------
//bool voDelimitedTextPreviewModel::useFirstLineAsAttributeNames() const
//{
//  Q_D(const voDelimitedTextPreviewModel);
//  return d->UseFirstLineAsAttributeNames;
//}

//// --------------------------------------------------------------------------
//void voDelimitedTextPreviewModel::setUseFirstLineAsAttributeNames(bool _arg)
//{
//  Q_D(voDelimitedTextPreviewModel);
//  if (d->UseFirstLineAsAttributeNames != _arg)
//    {
//    d->UseFirstLineAsAttributeNames = _arg;
//    if (d->InlineUpdate)
//      {
//      this->updatePreview();
//      }
//    }
//}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::transpose() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->Transpose;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setTranspose(bool _arg)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->Transpose != _arg)
    {
    d->Transpose = _arg;
    if (d->InlineUpdate)
      {
      this->updatePreview();
      }
    }
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfColumnMetaDataTypes() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfColumnMetaDataTypes;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfColumnMetaDataTypes(int _arg)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfColumnMetaDataTypes != _arg)
    {
    d->NumberOfColumnMetaDataTypes = _arg;
    if (d->InlineUpdate)
      {
      this->updatePreview();
      }
    }
}

// --------------------------------------------------------------------------
int voDelimitedTextPreviewModel::numberOfRowMetaDataTypes() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->NumberOfRowMetaDataTypes;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setNumberOfRowMetaDataTypes(int _arg)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->NumberOfRowMetaDataTypes != _arg)
    {
    d->NumberOfRowMetaDataTypes = _arg;
    if (d->InlineUpdate)
      {
      this->updatePreview();
      }
    }
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
}

// --------------------------------------------------------------------------
bool voDelimitedTextPreviewModel::inlineUpdate() const
{
  Q_D(const voDelimitedTextPreviewModel);
  return d->InlineUpdate;
}

// --------------------------------------------------------------------------
void voDelimitedTextPreviewModel::setInlineUpdate(bool _arg)
{
  Q_D(voDelimitedTextPreviewModel);
  if (d->InlineUpdate != _arg)
    {
    d->InlineUpdate = _arg;
    if (d->InlineUpdate)
      {
      this->updatePreview();
      }
    }
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
    voDelimitedTextPreviewModelPrivate::transposeTable(table);
    }

  // Build model (self)
  this->clear();

  int modelRowCount = table->GetNumberOfRows();
  if (d->NumberOfColumnMetaDataTypes > 0)
    {
    modelRowCount--; // Consider the header data
    }
  this->setRowCount(modelRowCount);

  int modelColumnCount = table->GetNumberOfColumns();
  if (d->NumberOfRowMetaDataTypes > 0)
    {
    modelColumnCount--; // Consider the header data
    }
  this->setColumnCount(modelColumnCount);

  for (vtkIdType cid = 0; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(table->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = 0; rid < column->GetNumberOfValues(); ++rid)
      {
      QString value = QString(column->GetValue(rid));
      int modelRowId = rid;
      if (d->NumberOfColumnMetaDataTypes > 0)
        {
        modelRowId--; // Consider the header data
        }
      int modelColumnId = cid;
      if (d->NumberOfRowMetaDataTypes > 0)
        {
        modelColumnId--; // Consider the header data
        }
      QStandardItem* currentItem = 0;
      QColor headerBackgroundColor = QPalette().color(QPalette::Window);
      if (cid < d->NumberOfRowMetaDataTypes)
        {
        if (cid == d->RowMetaDataTypeOfInterest)
          {
          this->setHeaderData(modelRowId, Qt::Vertical, value);
          }
        else
          {
          this->setItem(modelRowId, modelColumnId, (currentItem = new QStandardItem(value)));
          currentItem->setData(headerBackgroundColor, Qt::BackgroundColorRole);

          // Update horizontal header
          if (rid == d->ColumnMetaDataTypeOfInterest)
            {
            this->setHeaderData(modelColumnId, Qt::Horizontal, value);
            }
          }
        }
      else
        {
        if (rid < d->NumberOfColumnMetaDataTypes)
          {
          if (rid == d->ColumnMetaDataTypeOfInterest)
            {
            this->setHeaderData(modelColumnId, Qt::Horizontal, value);
            }
          else
            {
            this->setItem(modelRowId, modelColumnId, (currentItem = new QStandardItem(value)));
            currentItem->setData(headerBackgroundColor, Qt::BackgroundColorRole);
            }
          }
        else
          {
          this->setItem(modelRowId, modelColumnId, (currentItem = new QStandardItem(value)));
          }
        }
      if(currentItem)
        {
        currentItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
      }
    }
}

