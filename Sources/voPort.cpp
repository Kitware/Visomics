
// Visomics includes
#include "voPort.h"
#include "voAnalysis.h"

// --------------------------------------------------------------------------
voAnalysis* voPort::analysis()const
{
  return this->Analysis;
}

// --------------------------------------------------------------------------
void voPort::setAnalysis(voAnalysis* an)
{
  this->Analysis = an;
}

// --------------------------------------------------------------------------
QString voPort::name()const
{
  return this->Name;
}

// --------------------------------------------------------------------------
void voPort::setName(const QString& nm)
{
  this->Name = nm;
}

// --------------------------------------------------------------------------
vtkDataObject* voPort::data()
{
  if (this->Analysis)
    {
    return this->Analysis->output(this->Name);
    }
  return 0;
}
