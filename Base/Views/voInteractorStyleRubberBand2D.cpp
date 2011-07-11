
// VTK includes
#include "vtkObjectFactory.h"

// Visomics includes
#include "voInteractorStyleRubberBand2D.h"

// --------------------------------------------------------------------------
// voInteractorStyleRubberBand2D methods

// --------------------------------------------------------------------------
vtkStandardNewMacro(voInteractorStyleRubberBand2D);

// --------------------------------------------------------------------------
voInteractorStyleRubberBand2D::voInteractorStyleRubberBand2D()
{
}

// --------------------------------------------------------------------------
voInteractorStyleRubberBand2D::~voInteractorStyleRubberBand2D()
{
}

// --------------------------------------------------------------------------
void voInteractorStyleRubberBand2D::OnLeftButtonDown()
{
  Superclass::OnMiddleButtonDown();
}

// --------------------------------------------------------------------------
void voInteractorStyleRubberBand2D::OnLeftButtonUp()
{
  Superclass::OnMiddleButtonUp();
}

// --------------------------------------------------------------------------
void voInteractorStyleRubberBand2D::OnMiddleButtonDown()
{
  Superclass::OnRightButtonDown();
}

// --------------------------------------------------------------------------
void voInteractorStyleRubberBand2D::OnMiddleButtonUp()
{
  Superclass::OnRightButtonUp();
}

// --------------------------------------------------------------------------
void voInteractorStyleRubberBand2D::OnRightButtonDown()
{
  Superclass::OnLeftButtonDown();
}

// --------------------------------------------------------------------------
void voInteractorStyleRubberBand2D::OnRightButtonUp()
{
  Superclass::OnLeftButtonUp();
}
