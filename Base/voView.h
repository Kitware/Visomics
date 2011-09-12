

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

  voDataObject* dataObject()const;

  void setDataObject(voDataObject* dataObject);

  virtual QList<QAction*> actions();

  void saveScreenshot(const QString& fileName);
  void saveScreenshot(const QString& fileName, const QSize& size);

protected slots:
  void onSaveScreenshotActionTriggered();

protected:
  virtual void setupUi(QLayout * layout) = 0;

  virtual QWidget* mainWidget();

  virtual void setDataObjectInternal(const voDataObject& dataObject) = 0;

protected:
  QScopedPointer<voViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voView);
  Q_DISABLE_COPY(voView);
};

#endif
