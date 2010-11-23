
// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkDataObject.h>

// --------------------------------------------------------------------------
void voView::updateInternal()
{
  vtkDataObject* input_data = this->input().data();
  if (!input_data)
    {
    return;
    }

  vtkDataObject* obj = input_data->NewInstance();
  obj->ShallowCopy(input_data);
  this->Outputs["output"] = obj;
  obj->Delete();
}
