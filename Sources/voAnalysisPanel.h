
#ifndef __voAnalysisPanel_h
#define __voAnalysisPanel_h

#include <QWidget>

class Ui_voAnalysisPanel;
class voAnalysis;
class voView;

class voAnalysisPanel : public QWidget
{
  Q_OBJECT

public:
  voAnalysisPanel();
  virtual ~voAnalysisPanel();
  void SetAnalysis( voAnalysis * analysis ); 

  voView* currentView();

protected slots:

protected:
  Ui_voAnalysisPanel* ui;
  voAnalysis* Analysis;
};

#endif
