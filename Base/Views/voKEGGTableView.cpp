
// Qt includes
#include <QDebug>
#include <QLayout>
#include <QTableWidget>
//#include <QHeaderView>

// Visomics includes
#include "voApplication.h"
#include "voAnalysisDriver.h"
#include "voDataObject.h"
#include "voKEGGTableView.h"

// VTK includes
#include <vtkTable.h>
#include <vtkVariant.h>

// --------------------------------------------------------------------------
class voKEGGTableViewPrivate
{
public:
  voKEGGTableViewPrivate();

  QTableWidget* TableWidget;
};

// --------------------------------------------------------------------------
// voKEGGTableViewPrivate methods

// --------------------------------------------------------------------------
voKEGGTableViewPrivate::voKEGGTableViewPrivate()
{
  this->TableWidget = 0;
}

// --------------------------------------------------------------------------
// voKEGGTableView methods

// --------------------------------------------------------------------------
voKEGGTableView::voKEGGTableView(QWidget* newParent):
    Superclass(newParent), d_ptr(new voKEGGTableViewPrivate)
{
}

// --------------------------------------------------------------------------
voKEGGTableView::~voKEGGTableView()
{
  Q_D(voKEGGTableView);

  delete d->TableWidget;
}

// --------------------------------------------------------------------------
void voKEGGTableView::setupUi(QLayout *layout)
{
  Q_D(voKEGGTableView);

  d->TableWidget = new QTableWidget();
  layout->addWidget(d->TableWidget);

  QObject::connect(d->TableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
                   this, SLOT(onItemDoubleClicked(QTableWidgetItem*)));
}

// --------------------------------------------------------------------------
QString voKEGGTableView::hints()const
{
  return "<img src=\":/Icons/Bulb.png\">&nbsp;If you <b>double-click</b> on a pathway cell, a "
      "<b>KEGG Pathway</b> analysis will be run.";
}

// --------------------------------------------------------------------------
void voKEGGTableView::setDataObjectInternal(voDataObject *dataObject)
{
  Q_D(voKEGGTableView);

  vtkTable * table = vtkTable::SafeDownCast(dataObject->dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "voKEGGTableView - Failed to setDataObject - vtkTable data is expected !";
    return;
    }

  vtkIdType num_rows = table->GetNumberOfRows();
  vtkIdType num_cols = table->GetNumberOfColumns();
  d->TableWidget->setRowCount(num_rows);
  d->TableWidget->setColumnCount(num_cols - 1);
  for (vtkIdType c = 1; c < num_cols; ++c)
    {
    d->TableWidget->setHorizontalHeaderItem(c-1, new QTableWidgetItem(QString(table->GetColumnName(c))));
    for (vtkIdType r = 0; r < num_rows; ++r)
      {
      QStringList rawTexts = QString(table->GetValue(r, c).ToString()).split("#");
      QTableWidgetItem* item = new QTableWidgetItem();
      item->setText(rawTexts.at(0));
      item->setToolTip(rawTexts.value(1, ""));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // Item is view-only
      d->TableWidget->setItem(r, c-1, item);
      }
    }
  for (vtkIdType r = 0; r < num_rows; ++r)
    {
    d->TableWidget->setVerticalHeaderItem(r, new QTableWidgetItem(QString(table->GetValue(r, 0).ToString())));
    }
  d->TableWidget->resizeColumnsToContents();
}

// --------------------------------------------------------------------------
void voKEGGTableView::onItemDoubleClicked(QTableWidgetItem* item)
{
  if (item->text().startsWith("path:"))
    {
    QHash<QString, QVariant> parameters;
    parameters.insert("pathway_id", QVariant(item->text()));

    voApplication::application()->analysisDriver()->runAnalysisForCurrentInput("KEGG Pathway", parameters);
    }
}
