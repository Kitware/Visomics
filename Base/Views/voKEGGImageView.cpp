
// Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QScrollArea>
#include <QVariant>

// Visomics includes
#include "voDataObject.h"
#include "voKEGGImageView.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>

// --------------------------------------------------------------------------
class voKEGGImageViewPrivate
{
public:
  voKEGGImageViewPrivate();

  QLabel* Label;
  QScrollArea* ScrollArea;
};

// --------------------------------------------------------------------------
// voKEGGImageViewPrivate methods

// --------------------------------------------------------------------------
voKEGGImageViewPrivate::voKEGGImageViewPrivate()
{
  this->Label = 0;
  this->ScrollArea = 0;
}

// --------------------------------------------------------------------------
// voKEGGImageView methods

// --------------------------------------------------------------------------
voKEGGImageView::voKEGGImageView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voKEGGImageViewPrivate)
{
}

// --------------------------------------------------------------------------
voKEGGImageView::~voKEGGImageView()
{
  Q_D(voKEGGImageView);

  delete d->Label;
  delete d->ScrollArea;
}

// --------------------------------------------------------------------------
void voKEGGImageView::setupUi(QLayout *layout)
{
  Q_D(voKEGGImageView);

  d->ScrollArea = new QScrollArea();
  layout->addWidget(d->ScrollArea);

  d->Label = new QLabel();
}

// --------------------------------------------------------------------------
void voKEGGImageView::setDataObjectInternal(const voDataObject& dataObject)
{
  Q_D(voKEGGImageView);

  if (!dataObject.data().canConvert<QPixmap>())
    {
    qCritical() << "voKEGGImageView - Failed to setDataObject - dataObject does not contain a QPixmap";
    return;
    }
  QPixmap pixmap = dataObject.data().value<QPixmap>();

  d->Label->setPixmap(pixmap);

  d->ScrollArea->setWidget(d->Label); // Must be done last
}
