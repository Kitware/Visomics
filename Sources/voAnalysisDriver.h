#ifndef __voAnalysisDriver_h
#define __voAnalysisDriver_h

// Qt includes
#include <QScopedPointer>
#include <QObject>

class voAnalysis;
class voDataModelItem;
class voDataObject;
class voAnalysisDriverPrivate;

class voAnalysisDriver : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  voAnalysisDriver(QObject* newParent = 0);
  virtual ~voAnalysisDriver();

  void runAnalysis(const QString& analysisName, voDataModelItem* inputTarget);

  void runAnalysis(voAnalysis * analysis, voDataModelItem* inputTarget);

  static void addAnalysisToObjectModel(voAnalysis * analysis, voDataModelItem* insertLocation);

public slots:
  void runAnalysisForAllInputs(const QString& analysisName);

protected:
  QScopedPointer<voAnalysisDriverPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisDriver);
  Q_DISABLE_COPY(voAnalysisDriver);
};

#endif
