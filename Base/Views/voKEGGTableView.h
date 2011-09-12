

#ifndef __voKEGGTableView_h
#define __voKEGGTableView_h

// Qt includes
#include <QTableWidget>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkSmartPointer.h>

class voKEGGTableViewPrivate;

class voKEGGTableView : public voView
{
  Q_OBJECT;
public:
  typedef voView Superclass;
  voKEGGTableView(QWidget* newParent = 0);
  virtual ~voKEGGTableView();

  virtual QString hints()const;

  virtual QList<QAction*> actions();

protected slots:
  void onExportToCsvActionTriggered();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

public slots:
  void onItemDoubleClicked(QTableWidgetItem* item);

protected:
  QScopedPointer<voKEGGTableViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voKEGGTableView);
  Q_DISABLE_COPY(voKEGGTableView);
};

#endif
