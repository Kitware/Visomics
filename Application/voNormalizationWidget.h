
#ifndef __voNormalizationWidget_h
#define __voNormalizationWidget_h

// Qt includes
#include <QWidget>

class voNormalizationWidgetPrivate;

class voNormalizationWidget : public QWidget
{
  Q_OBJECT
public:
  typedef voNormalizationWidget Self;
  typedef QWidget Superclass;

  voNormalizationWidget(QWidget* newParent = 0);
  virtual ~voNormalizationWidget();

  QString selectedNormalizationMethod()const;

signals:
  void normalizationMethodSelected(const QString& methodName);

protected slots:
  void selectNormalizationMethod(const QString& methodName);

protected:
  QScopedPointer<voNormalizationWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voNormalizationWidget);
  Q_DISABLE_COPY(voNormalizationWidget);
};

#endif

