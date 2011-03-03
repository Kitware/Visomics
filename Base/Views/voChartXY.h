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

#include "vtkChart.h"
#include "vtkSmartPointer.h" // For SP ivars
#include <vector>
#include <map>

class vtkPlot;
class vtkAxis;
class vtkPlotGrid;
class vtkChartLegend;
class vtkTooltipItem;
class vtkVector2f;
class voChartXYPrivate; // Private class to keep my STL vector in...

class VTK_CHARTS_EXPORT voChartXY : public vtkChart
{
public:
  vtkTypeMacro(voChartXY, vtkChart);
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // Creates a 2D Chart object.
  static voChartXY *New();

  // Description:
  // Perform any updates to the item that may be necessary before rendering.
  // The scene should take care of calling this on all items before their
  // Paint function is invoked.
  virtual void Update();

  // Description:
  // Paint event for the chart, called whenever the chart needs to be drawn
  virtual bool Paint(vtkContext2D *painter);

  // Description:
  // Add a plot to the chart, defaults to using the name of the y column
  virtual vtkPlot * AddPlot(int type);

  // Description:
  // Adds a plot to the chart
  virtual vtkIdType AddPlot(vtkPlot* plot);

  // Description:
  // Remove the plot at the specified index, returns true if successful,
  // false if the index was invalid.
  virtual bool RemovePlot(vtkIdType index);

  // Description:
  // Remove all plots from the chart.
  virtual void ClearPlots();

  // Description:
  // Get the plot at the specified index, returns null if the index is invalid.
  virtual vtkPlot* GetPlot(vtkIdType index);

  // Description:
  // Get the number of plots the chart contains.
  virtual vtkIdType GetNumberOfPlots();

  // Description:
  // Figure out which quadrant the plot is in.
  int GetPlotCorner(vtkPlot *plot);

  // Description:
  // Figure out which quadrant the plot is in.
  void SetPlotCorner(vtkPlot *plot, int corner);

  // Description:
  // Get the axis specified by axisIndex. This is specified with the vtkAxis
  // position enum, valid values are vtkAxis::LEFT, vtkAxis::BOTTOM,
  // vtkAxis::RIGHT and vtkAxis::TOP.
  virtual vtkAxis* GetAxis(int axisIndex);

  // Description:
  // Set whether the chart should draw a legend.
  virtual void SetShowLegend(bool visible);

  // Description:
  // Get the vtkChartLegend object that will be displayed by the chart.
  virtual vtkChartLegend* GetLegend();

  // Description:
  // Get the number of axes in the current chart.
  virtual vtkIdType GetNumberOfAxes();

  // Description:
  // Request that the chart recalculates the range of its axes. Especially
  // useful in applications after the parameters of plots have been modified.
  virtual void RecalculateBounds();

  // Description:
  // If true then the axes will be drawn at the origin (scientific style).
  vtkSetMacro(DrawAxesAtOrigin, bool);
  vtkGetMacro(DrawAxesAtOrigin, bool);
  vtkBooleanMacro(DrawAxesAtOrigin, bool);

  // Description:
  // If true then the axes will be turned on and off depending upon whether
  // any plots are in that corner. Defaults to true.
  vtkSetMacro(AutoAxes, bool);
  vtkGetMacro(AutoAxes, bool);
  vtkBooleanMacro(AutoAxes, bool);

  // Description:
  // Border size of the axes that are hidden (vtkAxis::GetVisible())
  vtkSetMacro(HiddenAxisBorder, int);
  vtkGetMacro(HiddenAxisBorder, int);

  // Description:
  // Set the width fraction for any bar charts drawn in this chart. It is
  // assumed that all bar plots will use the same array for the X axis, and that
  // this array is regularly spaced. The delta between the first two x values is
  // used to calculated the width of the bars, and subdivided between each bar.
  // The default value is 0.8, 1.0 would lead to bars that touch.
  vtkSetMacro(BarWidthFraction, float);
  vtkGetMacro(BarWidthFraction, float);

  // Description:
  // Set the information passed to the tooltip
  virtual void SetTooltipInfo(const vtkContextMouseEvent &,
                              const vtkVector2f &,
                              int, vtkPlot*);

  // Description:
  // Add labels for the chart points
  void AddPointLabels( std::vector<std::string> labels );

  
//BTX
  // Description:
  // Return true if the supplied x, y coordinate is inside the item.
  virtual bool Hit(const vtkContextMouseEvent &mouse);

  // Description:
  // Mouse enter event.
  virtual bool MouseEnterEvent(const vtkContextMouseEvent &mouse);

  // Description:
  // Mouse move event.
  virtual bool MouseMoveEvent(const vtkContextMouseEvent &mouse);

  // Description:
  // Mouse leave event.
  virtual bool MouseLeaveEvent(const vtkContextMouseEvent &mouse);

  // Description:
  // Mouse button down event
  virtual bool MouseButtonPressEvent(const vtkContextMouseEvent &mouse);

  // Description:
  // Mouse button release event.
  virtual bool MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse);

  // Description:
  // Mouse wheel event, positive delta indicates forward movement of the wheel.
  virtual bool MouseWheelEvent(const vtkContextMouseEvent &mouse, int delta);
//ETX

//BTX
protected:
  voChartXY();
  ~voChartXY();

  // Description:
  // Recalculate the necessary transforms.
  void RecalculatePlotTransforms();

  // Description:
  // Calculate the optimal zoom level such that all of the points to be plotted
  // will fit into the plot area.
  void RecalculatePlotBounds();

  // Description:
  // Update the layout of the chart, this may require the vtkContext2D in order
  // to get font metrics etc. Initially this was added to resize the charts
  // according in response to the size of the axes.
  virtual bool UpdateLayout(vtkContext2D* painter);

  // Description:
  // Layout for the legend if it is visible. This is run after the axes layout
  // and will adjust the borders to account for the legend position.
  // \return The required space in the specified border.
  virtual int GetLegendBorder(vtkContext2D* painter, int axisPosition);

  // Description:
  // Called after the edges of the chart are decided, set the position of the
  // legend, depends upon its alignment.
  virtual void SetLegendPosition(const vtkRectf& rect);

  // Description:
  // The legend for the chart.
  vtkSmartPointer<vtkChartLegend> Legend;

  // Description:
  // The tooltip item for the chart - can be used to display extra information.
  vtkSmartPointer<vtkTooltipItem> Tooltip;

  // Description:
  // Does the plot area transform need to be recalculated?
  bool PlotTransformValid;

  // Description:
  // The origin of the box when selecting a region of the chart.
  float BoxOrigin[2];

  // Description:
  // The width and height of the selection box.
  float BoxGeometry[2];

  // Description:
  // Should the box be drawn (could be selection, zoom etc).
  bool DrawBox;

  // Description:
  // Should we draw the location of the nearest point on the plot?
  bool DrawNearestPoint;

  // Description:
  // Keep the axes drawn at the origin? This will attempt to keep the axes drawn
  // at the origin, i.e. 0.0, 0.0 for the chart. This is often the preferred
  // way of drawing scientific/mathematical charts.
  bool DrawAxesAtOrigin;

  // Description:
  // Should axes be turned on and off automatically - defaults to on.
  bool AutoAxes;

  // Description:
  // Size of the border when an axis is hidden
  int HiddenAxisBorder;

  // Description:
  // The fraction of the interval taken up along the x axis by any bars that are
  // drawn on the chart.
  float BarWidthFraction;

  // Description:
  // Indicate if the layout has changed in some way that would require layout
  // code to be called.
  bool LayoutChanged;

private:
  voChartXY(const voChartXY &); // Not implemented.
  void operator=(const voChartXY &);   // Not implemented.

  voChartXYPrivate *ChartPrivate; // Private class where I hide my STL containers

  // Description:
  // Figure out the spacing between the bar chart plots, and their offsets.
  void CalculateBarPlots();

  // Description:
  // Try to locate a point within the plots to display in a tooltip
  bool LocatePointInPlots(const vtkContextMouseEvent &mouse);

  // Description:
  // Remove the plot from the plot corners list.
  bool RemovePlotFromCorners(vtkPlot *plot);

  void ZoomInAxes(vtkAxis *x, vtkAxis *y, float *orign, float *max);

  vtkstd::vector<vtkstd::string> PointLabels;
  std::map< std::vector<float>, std::string > coordinateLabelMap;
 
//ETX
};

#endif //__voChartXY_h
