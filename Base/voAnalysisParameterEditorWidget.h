
#ifndef __voAnalysisParameterEditorWidget_h
#define __voAnalysisParameterEditorWidget_h

// Qt includes
#include <QWidget>

class QtProperty;
class voAnalysis;
class voAnalysisParameterEditorWidgetPrivate;

class voAnalysisParameterEditorWidget : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  voAnalysisParameterEditorWidget(QWidget* newParent = 0);
  virtual ~voAnalysisParameterEditorWidget();

public slots:
  void setAnalysis(voAnalysis* newAnalysis);

signals:
  void runAnalysisRequested(const QString& analysisName, const QHash<QString, QVariant>& parameters);
  void updateAnalysisRequested(voAnalysis* analysis, const QHash<QString, QVariant>& parameters);

protected slots:

  void reset();
  void updateAnalysis();
  void cloneAnalysis();
  void onLocalValueChanged(QtProperty *localProp, const QVariant &localValue);

protected:
  QScopedPointer<voAnalysisParameterEditorWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisParameterEditorWidget);
  Q_DISABLE_COPY(voAnalysisParameterEditorWidget);
};

#endif
