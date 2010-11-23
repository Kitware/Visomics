

#ifndef __voCorrelationGraphView_h
#define __voCorrelationGraphView_h

#include "voView.h"

#include <vtkSmartPointer.h>

class QVTKWidget;
class vtkGraphLayoutView;

class voCorrelationGraphView : public voView
{
public:
  voCorrelationGraphView();

  virtual QWidget* widget();

protected:
  virtual void updateInternal();

  vtkSmartPointer<vtkGraphLayoutView> GraphView;
  QVTKWidget*                         Widget;
};

#endif
