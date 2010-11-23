
// Visomics includes
#include "voAnalysis.h"
#include "voView.h"

// VTK includes
#include <vtkAlgorithm.h>
#include <vtkDataObject.h>

// --------------------------------------------------------------------------
voAnalysis::voAnalysis()
{
  this->Algorithm = 0;
}

// --------------------------------------------------------------------------
voAnalysis::~voAnalysis()
{
  if (this->Algorithm)
    {
    this->Algorithm->Delete();
    }
  QMap<QString, voView*>::iterator it, itEnd;
  itEnd = this->Views.end();
  for (it = this->Views.begin(); it != itEnd; ++it)
    {
    delete it.value();
    }
}

// --------------------------------------------------------------------------
voPort voAnalysis::input(const QString& str)const
{
  return this->Inputs[str];
}

// --------------------------------------------------------------------------
void voAnalysis::setInput(const QString& str, voPort p)
{
  this->Inputs[str] = p;
}

// --------------------------------------------------------------------------
void voAnalysis::update()
{
  //this->updateInputs();
  this->updateInternal();
}

// --------------------------------------------------------------------------
void voAnalysis::updateViews()
{
  foreach(voView* view, this->Views.values())
    {
    view->update();
    }
}

// --------------------------------------------------------------------------
void voAnalysis::updateInputs()
{
  foreach(voPort port, this->Inputs.values())
    {
    if (!port.analysis())
      {
      vtkGenericWarningMacro("Input analysis not defined.");
      continue;
      }
    port.analysis()->update();
    }
}

// --------------------------------------------------------------------------
void voAnalysis::updateInternal()
{
  QMap<QString, int>::iterator it, itEnd;
  itEnd = this->InputNameToIndex.end();
  for (it = this->InputNameToIndex.begin(); it != itEnd; ++it)
    {
    Algorithm->SetInputConnection(it.value(), this->Inputs[it.key()].data()->GetProducerPort());
    }

  this->Algorithm->Update();

  itEnd = this->OutputNameToIndex.end();
  for (it = this->OutputNameToIndex.begin(); it != itEnd; ++it)
    {
    this->Outputs[it.key()] = Algorithm->GetOutputDataObject(it.value());
    }
}

// --------------------------------------------------------------------------
void voAnalysis::addInput(const QString& str)
{
  this->Inputs[str] = voPort();
}

// --------------------------------------------------------------------------
vtkDataObject* voAnalysis::output(const QString& str)const
{
  //this->update();
  return this->Outputs[str];
}
