
#ifndef __voDelimitedTextPreview_h
#define __voDelimitedTextPreview_h

// Qt includes
#include <QTableView>

// Visomics includes
#include "voDelimitedTextPreviewModel.h"


class voDelimitedTextPreviewPrivate;

class voDelimitedTextPreview : public QTableView
{
  Q_OBJECT

public:
  typedef QTableView Superclass;
  voDelimitedTextPreview(QWidget* newParent = 0);
  virtual ~voDelimitedTextPreview();

public slots:

private:
  QScopedPointer<voDelimitedTextPreviewPrivate> d_ptr;

  Q_DECLARE_PRIVATE(voDelimitedTextPreview);
  Q_DISABLE_COPY(voDelimitedTextPreview);
};

#endif

