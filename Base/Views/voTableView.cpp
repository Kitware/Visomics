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
#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QTableView>
#include <QLayout>
#include <QStandardItemModel>
#include <QWidget>
#include <QHeaderView>

// Visomics includes
#include "voDataObject.h"
#include "voIOManager.h"
#include "voTableDataObject.h"
#include "voTableView.h"
#include "voUtils.h"

// VTK includes
#include <vtkIdTypeArray.h>
#include <vtkQtTableRepresentation.h>
#include <vtkQtTableView.h>
#include <vtkTable.h>
#include <vtkTypeTemplate.h> // for std::bad_cast

// --------------------------------------------------------------------------
class voTableViewPrivate
{
public:
  voTableViewPrivate();

  QTableView*        TableView;
  QStandardItemModel Model;
};

// --------------------------------------------------------------------------
// voTableViewPrivate methods

// --------------------------------------------------------------------------
voTableViewPrivate::voTableViewPrivate()
{
  this->TableView = 0;
}

// --------------------------------------------------------------------------
// voTableView methods

// --------------------------------------------------------------------------
voTableView::voTableView(QWidget* newParent):
    Superclass(newParent), d_ptr(new voTableViewPrivate)
{
}

// --------------------------------------------------------------------------
voTableView::~voTableView()
{
}

// --------------------------------------------------------------------------
QList<QAction*> voTableView::actions()
{
  QList<QAction*> actionList = this->Superclass::actions();

  QAction * exportToCsvAction = new QAction(QIcon(":/Icons/csv_text.png"), "Export as CSV", this);
  exportToCsvAction->setToolTip("Export current table view as CSV text file.");
  connect(exportToCsvAction, SIGNAL(triggered()), this, SLOT(onExportToCsvActionTriggered()));
  actionList << exportToCsvAction;

  return actionList;
}

// --------------------------------------------------------------------------
void voTableView::onExportToCsvActionTriggered()
{
  QString defaultFileName = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)
      + "/" + voUtils::cleanString(this->objectName()) + ".csv";

  QString fileName = QFileDialog::getSaveFileName(
        0, tr("Save table data..."), defaultFileName, "Comma Separated Value (*.csv)");
  if(fileName.isEmpty())
    {
    return;
    }
  vtkTable * table = vtkTable::SafeDownCast(this->dataObject()->dataAsVTKDataObject());
  Q_ASSERT(table);
  voIOManager::writeTableToCVSFile(table, fileName);
}

// --------------------------------------------------------------------------
void voTableView::setupUi(QLayout *layout)
{
  Q_D(voTableView);

  d->TableView = new QTableView();
  d->TableView->setModel(&d->Model);

  layout->addWidget(d->TableView);
}

namespace
{
// --------------------------------------------------------------------------
enum Roles
{
  SortRole = Qt::UserRole + 1
};
} // end of anonymous namespace

// --------------------------------------------------------------------------
void voTableView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voTableView);

  bool sortable;
  // dataObject should actually be a voTableDataObject, which has the sortable() property
  try
    {
    voTableDataObject& tableDataObject = dynamic_cast<voTableDataObject&>(const_cast<voDataObject&>(dataObject));
    sortable = tableDataObject.sortable();
    }
  catch (std::bad_cast&)
    {
    qCritical() << "voTableView - Failed to setDataObject - dataObject is not voTableDataObject";
    return;
    }

  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voTableView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  vtkIdType num_rows = table->GetNumberOfRows();
  vtkIdType num_cols = table->GetNumberOfColumns();
  vtkIdType colOffset = sortable ? 0 : 1;

  d->Model.setRowCount(static_cast<int>(num_rows));
  d->Model.setColumnCount(static_cast<int>(num_cols - colOffset));
  for (vtkIdType dataCol = colOffset; dataCol < num_cols; ++dataCol)
    {
    int modelCol = static_cast<int>(dataCol - colOffset);

    QString columnName(table->GetColumnName(dataCol));
    // unkown is used to encode columns with no name
    if (columnName == "unknown")
      columnName = "";

    d->Model.setHeaderData(modelCol, Qt::Horizontal, columnName);
    for (vtkIdType r = 0; r < num_rows; ++r)
      {
      QStandardItem* item = new QStandardItem();
      vtkVariant variant = table->GetValue(r, dataCol);
      item->setText(QString(variant.ToString()));
      if (sortable)
        {
        item->setData(variant.ToDouble(), SortRole);
        }
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // Item is view-only
      d->Model.setItem(static_cast<int>(r), modelCol, item);
      }
    }
  for (vtkIdType r = 0; r < num_rows; ++r)
    {
    if(sortable)
      {
      QStandardItem * item = d->Model.item(r, 0);
      item->setBackground(QPalette().color(QPalette::Window));
      item->setData(r, SortRole);
      d->Model.setHeaderData(static_cast<int>(r), Qt::Vertical, QString());
      }
    else
      {
      d->Model.setHeaderData(static_cast<int>(r), Qt::Vertical, QString(table->GetValue(r, 0).ToString()));
      }
    }

  d->TableView->horizontalHeader()->setMinimumSectionSize(120);
  d->TableView->resizeColumnsToContents();

  d->TableView->setSortingEnabled(sortable);

  QStandardItemModel* standardModel = qobject_cast<QStandardItemModel*>(d->TableView->model());
  if (standardModel)
    {
    standardModel->setSortRole(SortRole);
    }

  // Retrieve the selected subtable
  //vtkSmartPointer<vtkTable> ot = vtkSmartPointer<vtkTable>::New();
  //this->selectedTable(ot);
  //this->Outputs["output"] = ot;
}

// --------------------------------------------------------------------------
//void voTableView::selectedTable(vtkTable* table)
//{
//  vtkTable* t = vtkTable::SafeDownCast(this->input().data());
//  if (!t)
//    {
//    return;
//    }

//  QSet<vtkIdType> rows;
//  QSet<vtkIdType> cols;

//  // Always select the first column (headers)
//  cols << 0;

//  const QModelIndexList selected = this->View->selectionModel()->selectedIndexes();
//  if (selected.size() == 0)
//    {
//    table->ShallowCopy(t);
//    return;
//    }
//  foreach (QModelIndex ind, selected)
//    {
//    cols << ind.row();
//    rows << ind.column();
//    }

//  foreach (vtkIdType c, cols)
//    {
//    vtkAbstractArray* col = t->GetColumn(c);
//    vtkAbstractArray* new_col = col->NewInstance();
//    new_col->SetName(col->GetName());
//    new_col->SetNumberOfTuples(rows.size());
//    table->AddColumn(new_col);
//    int ind = 0;
//    foreach (vtkIdType r, rows)
//      {
//      new_col->InsertTuple(ind, r, col);
//      ++ind;
//      }
//    new_col->Delete();
//    }
//}
