
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
  for (vtkIdType tableRowItr = 0; tableRowItr < extendedTable->GetNumberOfColumnMetaDataTypes(); ++tableRowItr)
    {
    vtkStringArray * metadata = extendedTable->GetColumnMetaDataAsString(tableRowItr);
    Q_ASSERT(metadata);
    for(vtkIdType tableColItr = 0; tableColItr < metadata->GetNumberOfValues(); ++tableColItr)
      {
      int modelRowItr = tableRowItr;
      if (tableRowItr > extendedTable->GetColumnMetaDataTypeOfInterest())
        {
        modelRowItr--;
        }
      int modelColItr = tableColItr;
      if (extendedTable->HasRowMetaData())
        {
        modelColItr += extendedTable->GetNumberOfRowMetaDataTypes() - 1;
        }
      QString value = QString(metadata->GetValue(tableColItr));
      if(tableRowItr == extendedTable->GetColumnMetaDataTypeOfInterest())
        {
        if(tableColItr < 26)
          {
          value.prepend(QString("%1: ").arg(QChar('A' + tableColItr)));
          }
        d->Model.setHeaderData(modelColItr, Qt::Horizontal, value);
        }
      else
        {
        QStandardItem * currentItem = new QStandardItem(value);
        d->Model.setItem(modelRowItr, modelColItr, currentItem);
        currentItem->setData(headerBackgroundColor, Qt::BackgroundRole);
        currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
      }
    }

  // Set row meta data
  for(vtkIdType tableColItr = 0; tableColItr < extendedTable->GetNumberOfRowMetaDataTypes(); ++tableColItr)
    {
    vtkStringArray * metadata = extendedTable->GetRowMetaDataAsString(tableColItr);
    Q_ASSERT(metadata);
    for (vtkIdType tableRowItr = 0; tableRowItr <  metadata->GetNumberOfValues(); ++tableRowItr)
      {
      int modelColItr = tableColItr;
      if (tableColItr > extendedTable->GetRowMetaDataTypeOfInterest())
        {
        modelColItr--;
        }
      int modelRowItr = tableRowItr;
      if (extendedTable->HasColumnMetaData())
        {
        modelRowItr += extendedTable->GetNumberOfColumnMetaDataTypes() - 1;
        }
      QString value = QString(metadata->GetValue(tableRowItr));
      if(tableColItr == extendedTable->GetRowMetaDataTypeOfInterest())
        {
        value.prepend(QString("%1: ").arg(1 + tableRowItr));
        d->Model.setHeaderData(modelRowItr, Qt::Vertical, value);
        }
      else
        {
        QStandardItem * currentItem = new QStandardItem(value);
        d->Model.setItem(modelRowItr, modelColItr, currentItem);
        currentItem->setData(headerBackgroundColor, Qt::BackgroundRole);
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

  // Set upper-left "empty quadrant" headers to blank
  for (int cid = 0; cid < extendedTable->GetNumberOfRowMetaDataTypes() - 1; ++cid)
    {
    d->Model.setHeaderData(cid, Qt::Horizontal, QString(""));
    }
  for (int rid = 0; rid < extendedTable->GetNumberOfColumnMetaDataTypes() - 1; ++rid)
    {
    d->Model.setHeaderData(rid, Qt::Vertical, QString(""));
    }
}
