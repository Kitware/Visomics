
#ifndef __voNormalizerRegistry_h
#define __voNormalizerRegistry_h

// Qt includes
#include <QHash>
#include <QScopedPointer>
#include <QtGlobal>
#include <QVariant>

class QString;
class voNormalizerRegistryPrivate;
class vtkTable;

class voNormalizerRegistry
{
public:
  voNormalizerRegistry();
  virtual ~voNormalizerRegistry();

  typedef bool(*ApplyNormalizationFunction)(vtkTable*, const QHash<int, QVariant>&);
  void registerNormalizationMethod(const QString& methodName, ApplyNormalizationFunction function);
  
  bool applyNormalization(const QString& methodName, vtkTable * dataTable, const QHash<int, QVariant>& settings);
  
protected:
  QScopedPointer<voNormalizerRegistryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voNormalizerRegistry);
  Q_DISABLE_COPY(voNormalizerRegistry);
};


#endif

