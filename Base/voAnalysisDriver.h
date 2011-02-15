#ifndef __voAnalysisDriver_h
#define __voAnalysisDriver_h

// Qt includes
#include <QScopedPointer>
#include <QObject>
#include <QHash>

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

  void runAnalysis(const QString& analysisName, voDataModelItem* inputTarget, bool acceptDefaultParameter = false);

signals:
  void aboutToRunAnalysis(voAnalysis*);
  void analysisAddedToObjectModel(voAnalysis*);

public slots:
  void runAnalysisForAllInputs(const QString& analysisName, bool acceptDefaultParameter = false);

  void runAnalysisForCurrentInput(
    const QString& analysisName, const QHash<QString, QVariant>& parameters);

  void updateAnalysis(
    voAnalysis * analysis, const QHash<QString, QVariant>& parameters);

protected slots:

  void onAnalysisOutputSet(const QString& outputName, voDataObject* dataObject, voAnalysis* analysis);

protected:
  void runAnalysis(voAnalysis * analysis, voDataModelItem* inputTarget);

  static void addAnalysisToObjectModel(voAnalysis * analysis, voDataModelItem* insertLocation);

protected:
  QScopedPointer<voAnalysisDriverPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisDriver);
  Q_DISABLE_COPY(voAnalysisDriver);
};

#endif
