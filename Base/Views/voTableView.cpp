
// Qt includes
#include <QDebug>
#include <QTableView>
#include <QLayout>
#include <QStandardItemModel>
#include <QWidget>
#include <QHeaderView>

// Visomics includes
#include "voTableView.h"
#include "voDataObject.h"

// VTK includes
#include <vtkIdTypeArray.h>
#include <vtkQtTableRepresentation.h>
#include <vtkQtTableView.h>
#include <vtkTable.h>

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
void voTableView::setupUi(QLayout *layout)
{
  Q_D(voTableView);

  d->TableView = new QTableView();
  d->TableView->setModel(&d->Model);

  layout->addWidget(d->TableView);
}

// --------------------------------------------------------------------------
void voTableView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voTableView);

  vtkTable * table = vtkTable::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voTableView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  // Update the model if necessary
  //if (t->GetMTime() > this->LastModelMTime)
  //  {
    // Note: this will clear the current selection.
    vtkIdType num_rows = table->GetNumberOfRows();
    vtkIdType num_cols = table->GetNumberOfColumns();
    d->Model.setRowCount(static_cast<int>(num_rows));
    d->Model.setColumnCount(static_cast<int>(num_cols - 1));
    for (vtkIdType c = 1; c < num_cols; ++c)
      {
      d->Model.setHeaderData(static_cast<int>(c-1), Qt::Horizontal, QString(table->GetColumnName(c)));
      for (vtkIdType r = 0; r < num_rows; ++r)
        {
        QStandardItem* item = new QStandardItem(QString(table->GetValue(r, c).ToString()));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // Item is view-only
        d->Model.setItem(static_cast<int>(r), static_cast<int>(c-1), item);
        }
      }
    for (vtkIdType r = 0; r < num_rows; ++r)
      {
      d->Model.setHeaderData(static_cast<int>(r), Qt::Vertical, QString(table->GetValue(r, 0).ToString()));
      }
  //  this->LastModelMTime = table->GetMTime();
  //  }

    d->TableView->horizontalHeader()->setMinimumSectionSize(100);
    d->TableView->resizeColumnsToContents();

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
