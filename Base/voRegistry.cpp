
// Visomics includes
#include "voRegistry.h"

// VTK includes
#include <vtkTable.h>

//----------------------------------------------------------------------------
namespace
{
bool applyNoop(vtkTable * /*dataTable*/, const QHash<int, QVariant>& /*settings*/){ return true; }
} // end of Normalization namespace

//----------------------------------------------------------------------------
class voRegistryPrivate
{
public:
  QHash<QString, voRegistry::ApplyNormalizationFunction> MethodNameToFunctionMap;
};

//----------------------------------------------------------------------------
// voRegistry methods

//----------------------------------------------------------------------------
voRegistry::voRegistry():d_ptr(new voRegistryPrivate)
{
  this->registerMethod("No", applyNoop);
}

//----------------------------------------------------------------------------
voRegistry::~voRegistry()
{
}

//----------------------------------------------------------------------------
void voRegistry::registerMethod(const QString& methodName, ApplyNormalizationFunction function)
{
  Q_D(voRegistry);
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
bool voRegistry::apply(const QString& methodName, vtkTable * dataTable, const QHash<int, QVariant>& settings)
{
  Q_D(voRegistry);
  if (!d->MethodNameToFunctionMap.keys().contains(methodName))
    {
    return false;
    }
  return (*d->MethodNameToFunctionMap.value(methodName))(dataTable, settings);
}
