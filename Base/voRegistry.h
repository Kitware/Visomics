
#ifndef __voRegistry_h
#define __voRegistry_h

// Qt includes
#include <QHash>
#include <QScopedPointer>
#include <QtGlobal>
#include <QVariant>

class QString;
class voRegistryPrivate;
class vtkTable;

class voRegistry
{
public:
  voRegistry();
  virtual ~voRegistry();

  typedef bool(*ApplyNormalizationFunction)(vtkTable*, const QHash<int, QVariant>&);
  void registerMethod(const QString& methodName, ApplyNormalizationFunction function);
  
  bool apply(const QString& methodName, vtkTable * dataTable, const QHash<int, QVariant>& settings);
  
protected:
  QScopedPointer<voRegistryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voRegistry);
  Q_DISABLE_COPY(voRegistry);
};


#endif

