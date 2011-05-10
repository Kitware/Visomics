
// Visomics includes
#include "voLog2.h"
#include "voNormalizerRegistry.h"

// VTK includes
#include <vtkTable.h>

//----------------------------------------------------------------------------
namespace Normalization
{
bool applyNoop(vtkTable * /*dataTable*/, const QHash<int, QVariant>& /*settings*/){ return true; }
} // end of Normalization namespace

//----------------------------------------------------------------------------
class voNormalizerRegistryPrivate
{
public:
  QHash<QString, voNormalizerRegistry::ApplyNormalizationFunction> MethodNameToFunctionMap;
};

//----------------------------------------------------------------------------
// voNormalizerRegistry methods

//----------------------------------------------------------------------------
voNormalizerRegistry::voNormalizerRegistry():d_ptr(new voNormalizerRegistryPrivate)
{
  this->registerNormalizationMethod("No", Normalization::applyNoop);
  this->registerNormalizationMethod("Log2", Normalization::applyLog2);
}

//----------------------------------------------------------------------------
voNormalizerRegistry::~voNormalizerRegistry()
{
}

//----------------------------------------------------------------------------
void voNormalizerRegistry::registerNormalizationMethod(const QString& methodName, ApplyNormalizationFunction function)
{
  Q_D(voNormalizerRegistry);
  if (d->MethodNameToFunctionMap.keys().contains(methodName))
    {
    return;
    }
  if (!function)
    {
    return;
    }
  d->MethodNameToFunctionMap.insert(methodName, function);
}

//----------------------------------------------------------------------------  
bool voNormalizerRegistry::applyNormalization(const QString& methodName, vtkTable * dataTable, const QHash<int, QVariant>& settings)
{
  Q_D(voNormalizerRegistry);
  if (!d->MethodNameToFunctionMap.keys().contains(methodName))
    {
    return false;
    }
  return (*d->MethodNameToFunctionMap.value(methodName))(dataTable, settings);
}
