

#ifndef __voDynView_h
#define __voDynView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voDataObject;
class voDynViewPrivate;

class voDynView : public voView
{
  Q_OBJECT
  Q_PROPERTY(QString htmlFilePath READ htmlFilePath)
public:
  typedef voView Superclass;
  voDynView(QWidget* newParent = 0);
  virtual ~voDynView();

  QString htmlFilePath()const;

  virtual void initialize();

  QString viewName()const;

protected slots:
  void loadDataObject();

protected:
  virtual void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

  virtual QString stringify(const voDataObject& dataObject);

protected:
  QScopedPointer<voDynViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDynView);
  Q_DISABLE_COPY(voDynView);
};

#endif
