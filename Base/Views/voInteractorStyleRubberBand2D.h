/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/


#ifndef __voInteractorStyleRubberBand2D_h
#define __voInteractorStyleRubberBand2D_h

// VTK includes
#include <vtkInteractorStyleRubberBand2D.h>

class VTK_RENDERING_EXPORT voInteractorStyleRubberBand2D : public vtkInteractorStyleRubberBand2D
{
public:
  static voInteractorStyleRubberBand2D *New();
  vtkTypeMacro(voInteractorStyleRubberBand2D, vtkInteractorStyleRubberBand2D);

public:
  // This set of mouse bindings is consistant with what's implemented in vtkChart scenes

  // Left button = move scene
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();

  // Middle button = zoom
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();

  // Right button = select within scene
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();

  // Wheel = zoom
  //virtual void OnMouseWheelForward();
  //virtual void OnMouseWheelBackward();

protected:
  voInteractorStyleRubberBand2D();
  ~voInteractorStyleRubberBand2D();

private:
  voInteractorStyleRubberBand2D(const voInteractorStyleRubberBand2D&); // Not implemented
  void operator=(const voInteractorStyleRubberBand2D&); // Not implemented
};

#endif
