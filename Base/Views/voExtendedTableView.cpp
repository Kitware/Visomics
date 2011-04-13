
// Qt includes
#include <QDebug>
#include <QLayout>
#include <QTableView>
#include <QStandardItemModel>

// Visomics includes
#include "voExtendedTableView.h"
#include "voDataObject.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
class voExtendedTableViewPrivate
{
public:
  voExtendedTableViewPrivate();

  QTableView*        TableView;
  QStandardItemModel Model;
};

// --------------------------------------------------------------------------
// voExtendedTableViewPrivate methods

// --------------------------------------------------------------------------
voExtendedTableViewPrivate::voExtendedTableViewPrivate()
{
  this->TableView = 0;
}

// --------------------------------------------------------------------------
// voExtendedTableView methods

// --------------------------------------------------------------------------
voExtendedTableView::voExtendedTableView(QWidget* newParent):
    Superclass(newParent), d_ptr(new voExtendedTableViewPrivate)
{
}

// --------------------------------------------------------------------------
voExtendedTableView::~voExtendedTableView()
{
}

// --------------------------------------------------------------------------
void voExtendedTableView::setupUi(QLayout *layout)
{
  Q_D(voExtendedTableView);

  d->TableView = new QTableView();
  d->TableView->setModel(&d->Model);

  layout->addWidget(d->TableView);
}

// --------------------------------------------------------------------------
void voExtendedTableView::setDataObject(voDataObject *dataObject)
{
  Q_D(voExtendedTableView);

  if (!dataObject)
    {
    qCritical() << "voExtendedTableView - Failed to setDataObject - dataObject is NULL";
    return;
    }

  vtkExtendedTable * extendedTable = vtkExtendedTable::SafeDownCast(dataObject->data());
  if (!extendedTable)
    {
    qCritical() << "voExtendedTableView - Failed to setDataObject - vtkExtendedTable data is expected !";
    return;
    }

  int modelRowCount = extendedTable->GetTotalNumberOfRows();
  if (extendedTable->HasColumnMetaData())
    {
    modelRowCount--;
    }
  d->Model.setRowCount(modelRowCount);

  int modelColumnCount = extendedTable->GetTotalNumberOfColumns();
  if (extendedTable->HasRowMetaData())
    {
    modelColumnCount--;
    }
  d->Model.setColumnCount(modelColumnCount);

  QColor headerBackgroundColor = QPalette().color(QPalette::Window);

  // Set column meta data
  if(extendedTable->HasColumnMetaData())
    {
      {
      vtkStringArray * metadata = vtkStringArray::SafeDownCast(extendedTable->GetColumnMetaDataOfInterest());
      Q_ASSERT(metadata);
      for(int i = 0; i < metadata->GetNumberOfValues(); ++i)
        {
        int headerId = i;
        if (extendedTable->HasRowMetaData())
          {
          headerId += extendedTable->GetNumberOfRowMetaDataTypes() - 1;
          }
        QString value = QString(metadata->GetValue(i));
        d->Model.setHeaderData(headerId, Qt::Horizontal, value);
        }
      }
    // TODO Consider ColumnMetaDataTypeOfInterest
    for (int metadataType = 1; metadataType < extendedTable->GetNumberOfColumnMetaDataTypes(); ++metadataType)
      {
      vtkStringArray * metadata = vtkStringArray::SafeDownCast(extendedTable->GetColumnMetaData(metadataType));
      Q_ASSERT(metadata);
      for(int i = 0; i < metadata->GetNumberOfValues(); ++i)
        {
        int modelColumnId = i;
        if (extendedTable->HasRowMetaData())
          {
          modelColumnId += extendedTable->GetNumberOfRowMetaDataTypes() - 1;
          }
        int modelRowId = metadataType;
        if (extendedTable->HasColumnMetaData())
          {
          modelRowId--;
          }
        QString value = QString(metadata->GetValue(i));
        QStandardItem * currentItem = new QStandardItem(value);
        d->Model.setItem(modelRowId, modelColumnId, currentItem);
        currentItem->setData(headerBackgroundColor, Qt::BackgroundColorRole);
        currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
      }
    }

  // Set row meta data
  if(extendedTable->HasRowMetaData())
    {
      {
      vtkStringArray * metadata = vtkStringArray::SafeDownCast(extendedTable->GetRowMetaDataOfInterest());
      Q_ASSERT(metadata);
      for(int i = 0; i < metadata->GetNumberOfValues(); ++i)
        {
        int headerId = i;
        if (extendedTable->HasColumnMetaData())
          {
          headerId += extendedTable->GetNumberOfColumnMetaDataTypes() - 1;
          }
        QString value = QString(metadata->GetValue(i));
        d->Model.setHeaderData(headerId, Qt::Vertical, value);
        }
      }
    // TODO Consider RowMetaDataTypeOfInterest
    for (int metadataType = 1; metadataType < extendedTable->GetNumberOfRowMetaDataTypes(); ++metadataType)
      {
      vtkStringArray * metadata = vtkStringArray::SafeDownCast(extendedTable->GetRowMetaData(metadataType));
      Q_ASSERT(metadata);
      for(int i = 0; i < metadata->GetNumberOfValues(); ++i)
        {
        int modelColumnId = metadataType;
        if (extendedTable->HasRowMetaData())
          {
          modelColumnId--;
          }
        int modelRowId = i;
        if (extendedTable->HasColumnMetaData())
          {
          modelRowId += extendedTable->GetNumberOfColumnMetaDataTypes() - 1;
          }
        QString value = QString(metadata->GetValue(i));
        QStandardItem * currentItem = new QStandardItem(value);
        d->Model.setItem(modelRowId, modelColumnId, currentItem);
        currentItem->setData(headerBackgroundColor, Qt::BackgroundColorRole);
        currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
      }
    }

  // Set Data
  vtkTable * data = extendedTable->GetData();
  if (data)
    {
    for (int cid = 0; cid < data->GetNumberOfColumns(); ++cid)
      {
      vtkDoubleArray * dataColumn = vtkDoubleArray::SafeDownCast(data->GetColumn(cid));
      Q_ASSERT(dataColumn);
      for (int rid = 0; rid < dataColumn->GetNumberOfTuples(); ++rid)
        {
        int modelColumnId = cid;
        if (extendedTable->HasRowMetaData())
          {
          modelColumnId += extendedTable->GetNumberOfRowMetaDataTypes() - 1;
          }
        int modelRowId = rid;
        if (extendedTable->HasColumnMetaData())
          {
          modelRowId += extendedTable->GetNumberOfColumnMetaDataTypes() - 1;
          }
        double value = dataColumn->GetValue(rid);
        QStandardItem * currentItem = new QStandardItem(QString::number(value));
        d->Model.setItem(modelRowId, modelColumnId, currentItem);
        currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
      }
    }

}

