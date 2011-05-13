
#ifndef __voAnalysisFactory_h
#define __voAnalysisFactory_h

// Qt includes
#include <QScopedPointer>
#include <QString>

class voAnalysisFactoryPrivate;
class voAnalysis;

class voAnalysisFactory
{

public:
  voAnalysisFactory();
  virtual ~voAnalysisFactory();

  /// Given the name of a voAnalysis subclass, return a new instance of the analysis.
  virtual voAnalysis* createAnalysis(const QString& className);

  /// Return list of registered analysis names
  QStringList registeredAnalysisNames() const;

  /// If any, return the \a analysisName associated with \a analysisPrettyName
  QString analysisNameFromPrettyName(const QString& analysisPrettyName) const;

  /// Return list of registered analysis pretty names
  QStringList registeredAnalysisPrettyNames() const;

protected:

  /// Register an analysis
  template<typename AnalysisClassType>
  void registerAnalysis(const QString& analysisPrettyName);

protected:
  QScopedPointer<voAnalysisFactoryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisFactory);
  Q_DISABLE_COPY(voAnalysisFactory);
};

#endif
