

#ifndef __voPCAProjectionPlot_h
#define __voPCAProjectionPlot_h

// Qt includes
#include <QMap>
#include <QString>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkVariant.h>

class QVTKWidget;
class vtkChartXY;
class vtkContextView;
class vtkTable;

class voPCAProjectionPlot : public voView
{
public:
  voPCAProjectionPlot();

  virtual QWidget* widget();

protected:
  virtual void updateInternal();

  QMap<vtkVariant, vtkSmartPointer<vtkTable> > splitTable(vtkTable* t, const char* column);

  vtkSmartPointer<vtkContextView> ChartView;
  vtkSmartPointer<vtkChartXY> Chart;
  QVTKWidget* Widget;
};

#endif
