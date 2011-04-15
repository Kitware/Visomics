

#ifndef __voExtendedTableView_h
#define __voExtendedTableView_h

// Qt includes
#include <QStandardItemModel>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkSmartPointer.h>

class voExtendedTableViewPrivate;
class QTableView;
class vtkTable;

class voExtendedTableView : public voView
{
  Q_OBJECT;
public:
  typedef voView Superclass;
  voExtendedTableView(QWidget* newParent = 0);
  virtual ~voExtendedTableView();

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

protected:
  QScopedPointer<voExtendedTableViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voExtendedTableView);
  Q_DISABLE_COPY(voExtendedTableView);
};

#endif
