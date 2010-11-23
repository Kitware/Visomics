
// Qt includes
#include <QTableView>
#include <QLayout>
#include <QStandardItemModel>
#include <QWidget>

// Visomics includes
#include "voTableView.h"

// VTK includes
#include <vtkIdTypeArray.h>
#include <vtkQtTableRepresentation.h>
#include <vtkQtTableView.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
voTableView::voTableView()
{
  this->LastModelMTime = 0;
  this->View = new QTableView();
  this->View->setModel(&this->Model);
  this->addInput("input");
}

// --------------------------------------------------------------------------
voTableView::~voTableView()
{
  delete this->View;
}

// --------------------------------------------------------------------------
QWidget* voTableView::widget()
{
  return this->View;
}

// --------------------------------------------------------------------------
void voTableView::updateInternal()
{
  vtkTable* t = vtkTable::SafeDownCast(this->input().data());
  if (!t)
    {
    return;
    }

  // Update the model if necessary
  if (t->GetMTime() > this->LastModelMTime)
    {
    // Note: this will clear the current selection.
    vtkIdType num_rows = t->GetNumberOfRows();
    vtkIdType num_cols = t->GetNumberOfColumns();
    this->Model.setRowCount(static_cast<int>(num_cols - 1));
    this->Model.setColumnCount(static_cast<int>(num_rows));
    for (vtkIdType r = 0; r < num_rows; ++r)
      {
      this->Model.setHeaderData(static_cast<int>(r), Qt::Horizontal, QString(t->GetValue(r, 0).ToString()));
      for (vtkIdType c = 1; c < num_cols; ++c)
        {
        QStandardItem* item = new QStandardItem(QString(t->GetValue(r, c).ToString()));
        this->Model.setItem(static_cast<int>(c), static_cast<int>(r), item);
        }
      }
    for (vtkIdType c = 1; c < num_cols; ++c)
      {
      this->Model.setHeaderData(static_cast<int>(c), Qt::Vertical, QString(t->GetColumnName(c)));
      }
    this->View->hideRow(0);
    this->LastModelMTime = t->GetMTime();
    }

  // Retrieve the selected subtable
  vtkSmartPointer<vtkTable> ot = vtkSmartPointer<vtkTable>::New();
  this->selectedTable(ot);
  this->Outputs["output"] = ot;
}

// --------------------------------------------------------------------------
void voTableView::selectedTable(vtkTable* table)
{
  vtkTable* t = vtkTable::SafeDownCast(this->input().data());
  if (!t)
    {
    return;
    }

  QSet<vtkIdType> rows;
  QSet<vtkIdType> cols;

  // Always select the first column (headers)
  cols << 0;

  const QModelIndexList selected = this->View->selectionModel()->selectedIndexes();
  if (selected.size() == 0)
    {
    table->ShallowCopy(t);
    return;
    }
  foreach (QModelIndex ind, selected)
    {
    cols << ind.row();
    rows << ind.column();
    }

  foreach (vtkIdType c, cols)
    {
    vtkAbstractArray* col = t->GetColumn(c);
    vtkAbstractArray* new_col = col->NewInstance();
    new_col->SetName(col->GetName());
    new_col->SetNumberOfTuples(rows.size());
    table->AddColumn(new_col);
    int ind = 0;
    foreach (vtkIdType r, rows)
      {
      new_col->InsertTuple(ind, r, col);
      ++ind;
      }
    new_col->Delete();
    }
}
