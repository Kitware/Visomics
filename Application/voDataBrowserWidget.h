

#ifndef __voDataBrowserWidget_h
#define __voDataBrowserWidget_h

// Qt includes
#include <QTreeView>

class voAnalysis;

class voDataBrowserWidget : public QTreeView
{
  Q_OBJECT
public:
  typedef QTreeView Superclass;
  voDataBrowserWidget(QWidget* newParent = 0);
  virtual ~voDataBrowserWidget();

public slots:
  void setActiveAnalysis(voAnalysis* analysis);
};

#endif

