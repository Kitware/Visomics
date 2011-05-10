
// Qt includes
//#include <QHash>
//#include <QVariant>

// Visomics includes
#include "voLog2.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkTable.h>

// STD includes
#include <cmath>


namespace Normalization
{

//------------------------------------------------------------------------------
bool applyLog2(vtkTable * dataTable, const QHash<int, QVariant>& settings)
{
  Q_UNUSED(settings);
  if (!dataTable)
    {
    return false;
    }

  for (int cid = 0; cid < dataTable->GetNumberOfColumns(); ++cid)
    {
    vtkDoubleArray * column = vtkDoubleArray::SafeDownCast(dataTable->GetColumn(cid));
    if (!column)
      {
      continue;
      }
    for (int rid = 0; rid < column->GetNumberOfTuples() * column->GetNumberOfComponents(); ++rid)
      {
      column->SetValue(rid, std::log(column->GetValue(rid)) / log(2.0));
      }
    }
  dataTable->Modified();

  return true;
}

} // end of Normalization namespace
