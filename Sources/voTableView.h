

#ifndef __voTableView_h
#define __voTableView_h

// Qt includes
#include <QStandardItemModel>

// Visomics includes
#include <voView.h>

// VTK includes
#include <vtkSmartPointer.h>

class QTableView;
class vtkTable;

class voTableView : public voView
{
public:
  voTableView();
  ~voTableView();

  virtual QWidget* widget();

protected:
  virtual void selectedTable(vtkTable* table);

  virtual void updateInternal();

  QTableView* View;
  QStandardItemModel Model;
  unsigned long LastModelMTime;
};

#endif
