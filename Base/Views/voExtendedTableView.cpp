
// Qt includes
#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>

// Visomics includes
#include "voDataObject.h"
#include "voExtendedTableView.h"
#include "voIOManager.h"
#include "voUtils.h"
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
QList<QAction*> voExtendedTableView::actions()
{
  QList<QAction*> actionList = this->Superclass::actions();

  QAction * exportToCsvAction = new QAction(QIcon(":/Icons/csv_text.png"), "Export as CSV", this);
  exportToCsvAction->setToolTip("Export current table view as CSV text file.");
  connect(exportToCsvAction, SIGNAL(triggered()), this, SLOT(onExportToCsvActionTriggered()));
  actionList << exportToCsvAction;

  return actionList;
}

// --------------------------------------------------------------------------
void voExtendedTableView::onExportToCsvActionTriggered()
{
  QString defaultFileName = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)
      + "/" + voUtils::cleanString(this->objectName()) + ".csv";

  QString fileName = QFileDialog::getSaveFileName(
        0, tr("Save table data..."), defaultFileName, "Comma Separated Value (*.csv)");
  if(fileName.isEmpty())
    {
    return;
    }
  vtkExtendedTable * extendedTable =
      vtkExtendedTable::SafeDownCast(this->dataObject()->dataAsVTKDataObject());
  Q_ASSERT(extendedTable);
  voIOManager::writeTableToCVSFile(extendedTable->GetDataWithRowHeader(), fileName);
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
void voExtendedTableView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voExtendedTableView);

  vtkExtendedTable * extendedTable = vtkExtendedTable::SafeDownCast(dataObject.dataAsVTKDataObject());
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
        value.prepend(QString("%1: ").arg(voUtils::counterIntToAlpha(tableColItr)));
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

  // Set column metadata labels
  for (int modelRowId = 0; modelRowId < extendedTable->GetColumnMetaDataLabels()->GetNumberOfValues() - 1; ++modelRowId)
    {
    int tableRowId = modelRowId;
    if (tableRowId >= extendedTable->GetColumnMetaDataTypeOfInterest())
      {
      tableRowId++;
      }
    QString value = QString(extendedTable->GetColumnMetaDataLabels()->GetValue(tableRowId));
    value.prepend(QString("%1: ").arg(voUtils::counterIntToAlpha(modelRowId)));
    d->Model.setHeaderData(modelRowId, Qt::Vertical, value);
    }

  // Set row metadata labels
  for (int modelColumnId = 0; modelColumnId < extendedTable->GetRowMetaDataLabels()->GetNumberOfValues() - 1; ++modelColumnId)
    {
    int tableColumnId = modelColumnId;
    if (tableColumnId >= extendedTable->GetRowMetaDataTypeOfInterest())
      {
      tableColumnId++;
      }
    QString value = QString(extendedTable->GetRowMetaDataLabels()->GetValue(tableColumnId));
    value.prepend(QString("%1: ").arg(1 + modelColumnId));
    d->Model.setHeaderData(modelColumnId, Qt::Horizontal, value);
    }

  // Expand column widths to fit long labels or data
  d->TableView->horizontalHeader()->setMinimumSectionSize(100);
  d->TableView->resizeColumnsToContents();
}
