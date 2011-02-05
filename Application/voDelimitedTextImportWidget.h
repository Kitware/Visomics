
#ifndef __voDelimitedTextImportWidget_h
#define __voDelimitedTextImportWidget_h

// Qt includes
#include <QWidget>

class voDelimitedTextImportWidgetPrivate;

class voDelimitedTextImportWidget : public QWidget
{
  Q_OBJECT
public:
  public:
  typedef QWidget Superclass;
  voDelimitedTextImportWidget(QWidget* newParent = 0);
  virtual ~voDelimitedTextImportWidget();

protected:
  QScopedPointer<voDelimitedTextImportWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDelimitedTextImportWidget);
  Q_DISABLE_COPY(voDelimitedTextImportWidget);
};

#endif

