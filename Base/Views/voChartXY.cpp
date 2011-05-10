/*=========================================================================

  Program:   Visualization Toolkit
  Module:    voChartXY.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "voChartXY.h"

#include "vtkAxis.h"
#include "vtkContextMouseEvent.h"
#include "vtkPlot.h"
#include "vtkTooltipItem.h"
#include "vtkTable.h"
#include "vtkVariant.h"
#include "vtkVector.h"

#include "vtkObjectFactory.h"
#include "vtksys/ios/sstream"

// My STL containers
#include <vector>
#include <map>


//-----------------------------------------------------------------------------
vtkStandardNewMacro(voChartXY);

//-----------------------------------------------------------------------------
voChartXY::voChartXY() : vtkChartXY() {}

voChartXY::~voChartXY() {} //base class destuctor will be called automatically

//-----------------------------------------------------------------------------

void voChartXY::AddPointLabels( std::vector< std::string> labels )
{
  PointLabels = labels;

  //Assumes there is ONLY one plotline
  vtkTable* data = this->GetPlot(0)->GetInput(); 
  
  unsigned  int rows = data->GetNumberOfRows();

  //unsigned int cols = data->GetNumberOfColumns();
  //unsigned int numberOfLabels = labels.size();
  //std::cout << "Number Of Columns: " << cols << std::endl;
  //std::cout << "Number Of Rows: " << rows << std::endl;
  //std::cout << "Number Of Labels: " << numberOfLabels << std::endl;
  // BIG ASSUMPTION:
  // First column contains the labels
  // Second and Thrid column contains the 2D coordinates
 for ( unsigned int i=0 ; i < rows ; i++ )
    {
    std::vector<float> vector;
    vector.push_back(data->GetValue(i,1).ToFloat());
    vector.push_back(data->GetValue(i,2).ToFloat());
    //std::cout << "Vector values: " << vector[0] << "\t" << vector[1] <<  "\t" << labels[i] << std::endl;
    this->CoordinateLabelMap[vector] =labels[i];
    }
}

void voChartXY::SetTooltipInfo(const vtkContextMouseEvent& mouse,
                                const vtkVector2f &plotPos,
                                int vtkNotUsed(seriesIndex), vtkPlot* vtkNotUsed(plot))
{
  vtkStdString label;
  // Check for group names if we are plotting stacked bars
  //
  /*
  if (seriesIndex > 0)
    {
    vtkPlotBar *bar = vtkPlotBar::SafeDownCast(plot);
    if (bar && bar->GetGroupName())
      {
      label += vtkStdString(bar->GetGroupName()) + ": ";
      }
    }
  label += plot->GetLabel(seriesIndex);
  */

  // If axes are set to logarithmic scale we need to convert the
  // axis value using 10^(axis value)
  vtksys_ios::ostringstream ostr;
  ostr.imbue(vtkstd::locale::classic());
  ostr.setf(ios::fixed, ios::floatfield);
  std::vector<float> vector;
  vector.push_back( plotPos[0] );
  vector.push_back( plotPos[1] );

  //Do something more robust here

  std::map<std::vector<float>, std::string>::iterator lowerBoundIterator;
  lowerBoundIterator = this->CoordinateLabelMap.lower_bound(vector);
  std::string lowerBoundFoundLabel = (*lowerBoundIterator).second;

  std::string upperBoundFoundLabel;
  std::map<std::vector<float>, std::string>::iterator upperBoundIterator;
  upperBoundIterator = this->CoordinateLabelMap.upper_bound(vector);
  if ( upperBoundIterator != this->CoordinateLabelMap.end() )
    {
    upperBoundFoundLabel = (*upperBoundIterator).second;
    }

  if ( lowerBoundFoundLabel == upperBoundFoundLabel )
    {
    //float key value rounding off discrepancy
    label = "";
    }
  else
    {
    label = lowerBoundFoundLabel; 
    }

  //std::cout << "PlotPosition: \t" << plotPos[0] << "\t" << plotPos[1] << "\t" << label << std::endl;
  ostr << label << ": ";
  ostr.precision(this->GetAxis(vtkAxis::BOTTOM)->GetPrecision());
  ostr << (this->GetAxis(vtkAxis::BOTTOM)->GetLogScale()?
    pow(double(10.0), double(plotPos.X())):
    plotPos.X());
  ostr << ",  ";
  ostr.precision(this->GetAxis(vtkAxis::LEFT)->GetPrecision());
  ostr << (this->GetAxis(vtkAxis::LEFT)->GetLogScale()?
    pow(double(10.0), double(plotPos.Y())):
    plotPos.Y());
  this->Tooltip->SetText(ostr.str().c_str());
  this->Tooltip->SetPosition(mouse.ScreenPos[0]+2,
                             mouse.ScreenPos[1]+2);
}
