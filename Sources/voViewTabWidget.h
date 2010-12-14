
#ifndef __voViewTabWidget_h
#define __voViewTabWidget_h

// Qt includes
#include <QScopedPointer>
#include <QTabWidget>

class voViewTabWidgetPrivate;
class voAnalysis;
class voView;

class voViewTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  typedef QTabWidget Superclass;
  voViewTabWidget(QWidget * newParent = 0);
  virtual ~voViewTabWidget();

public slots:
  void createView(const QString& objectUuid, voView * newView);

protected slots:

  void onTabCloseRequested(int index);

protected:
  QScopedPointer<voViewTabWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voViewTabWidget);
  Q_DISABLE_COPY(voViewTabWidget);

};

#endif
