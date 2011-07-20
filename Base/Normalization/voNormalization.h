
#ifndef __voNormalization_h
#define __voNormalization_h

// Qt includes
#include <QHash>
#include <QVariant>

class vtkTable;
//template <class Key, class T> class QHash

//------------------------------------------------------------------------------
namespace Normalization
{

  bool applyLog2(vtkTable * dataTable, const QHash<int, QVariant>& settings);
  
} // end of Normalization namespace

#endif

