

#ifndef __voTableView_h
#define __voTableView_h

// Qt includes
#include <QStandardItemModel>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkSmartPointer.h>

class voTableViewPrivate;
class QTableView;
class vtkTable;

class voTableView : public voView
{
  Q_OBJECT;
public:
  typedef voView Superclass;
  voTableView(QWidget* newParent = 0);
  virtual ~voTableView();

  virtual QList<QAction*> actions();

protected slots:
  void onExportToCsvActionTriggered();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

protected:
  QScopedPointer<voTableViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTableView);
  Q_DISABLE_COPY(voTableView);
};

#endif
