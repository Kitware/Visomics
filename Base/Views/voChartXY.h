/*=========================================================================

  Program:   Visualization Toolkit
  Module:    voChartXY.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME voChartXY - Factory class for drawing XY charts
//
// .SECTION Description
// This class a modification of vtkChartXY to pass labels to data points and 
// display the labels in tooltip
//
// This class only works for a single plot in a Chart
//
// Not TESTED WELL YET.
//
// .SECTION See Also
// vtkBarChartActor

#ifndef __voChartXY_h
#define __voChartXY_h

#include "vtkChartXY.h"

#include <vector>
#include <map>

class vtkPlot;
class vtkAxis;
class vtkPlotGrid;
class vtkChartLegend;
class vtkTooltipItem;
class vtkVector2f;

class VTK_CHARTS_EXPORT voChartXY : public vtkChartXY
{
public:
  vtkTypeMacro(voChartXY, vtkChartXY);

  // Description:
  // Creates a 2D Chart object.
  static voChartXY *New();

  // Description:
  // Add labels for the chart points
  void AddPointLabels( std::vector<std::string> labels );

  // Description:
  // Set the information passed to the tooltip
  virtual void SetTooltipInfo(const vtkContextMouseEvent &,
                              const vtkVector2f &,
                              int, vtkPlot*);

//BTX
protected:
  voChartXY();
  ~voChartXY();

private:
  voChartXY(const voChartXY &); // Not implemented.
  void operator=(const voChartXY &);   // Not implemented.

  vtkstd::vector<vtkstd::string> PointLabels;
  std::map< std::vector<float>, std::string > coordinateLabelMap;
 
//ETX
};

#endif //__voChartXY_h
