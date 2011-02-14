
#ifndef __voViewStackedWidget_h
#define __voViewStackedWidget_h

// Qt includes
#include <QScopedPointer>
#include <QStackedWidget>

class voViewStackedWidgetPrivate;
class voAnalysis;
class voView;

class voViewStackedWidget : public QStackedWidget
{
  Q_OBJECT

public:
  typedef QStackedWidget Superclass;
  voViewStackedWidget(QWidget * newParent = 0);
  virtual ~voViewStackedWidget();

public slots:
  void addView(const QString& objectUuid, voView * newView);

protected:
  QScopedPointer<voViewStackedWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voViewStackedWidget);
  Q_DISABLE_COPY(voViewStackedWidget);

};

#endif
