

#ifndef __voView_h
#define __voView_h

// Qt includes
#include <QWidget>
#include <QList>

class voDataObject;

class voView : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  voView(QWidget* newParent = 0);
  virtual ~voView();

  void initialize();

  virtual void setDataObject(voDataObject* dataObject) = 0;

protected:

  virtual void setupUi(QLayout * layout) = 0;
};

#endif
