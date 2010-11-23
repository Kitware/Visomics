

#ifndef __voDataBrowserWidget_h
#define __voDataBrowserWidget_h

// Qt includes
#include <QTreeWidget>

class voDataBrowserWidget : public QTreeWidget
{
  Q_OBJECT
public:
  typedef QTreeWidget Superclass;
  voDataBrowserWidget(QWidget* newParent);
  virtual ~voDataBrowserWidget();
 
};

#endif

