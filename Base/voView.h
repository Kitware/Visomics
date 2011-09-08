

#ifndef __voView_h
#define __voView_h

// Qt includes
#include <QScopedPointer>
#include <QWidget>

class voDataObject;
class voViewPrivate;

class voView : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  voView(QWidget* newParent = 0);
  virtual ~voView();

  virtual void initialize();

  virtual QString hints()const;

  virtual void setDataObject(voDataObject* dataObject) = 0;

protected:

  virtual void setupUi(QLayout * layout) = 0;

protected:
  QScopedPointer<voViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voView);
  Q_DISABLE_COPY(voView);
};

#endif
