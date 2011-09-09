
// Qt includes
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QSharedDataPointer>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkDataObject.h>

//----------------------------------------------------------------------------
class voViewPrivate
{
public:
  voDataObject * DataObject;
};

// --------------------------------------------------------------------------
// voViewPrivate methods

// --------------------------------------------------------------------------
// voView methods

// --------------------------------------------------------------------------
voView::voView(QWidget* newParent) : Superclass(newParent), d_ptr(new voViewPrivate)
{
}

// --------------------------------------------------------------------------
voView::~voView()
{
}

// --------------------------------------------------------------------------
void voView::initialize()
{
  QVBoxLayout * verticalLayout = new QVBoxLayout(this);
  verticalLayout->setSpacing(0);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  this->setupUi(verticalLayout);
  QLabel * hintsLabel = new QLabel(this->hints());
  hintsLabel->setWordWrap(true);
  hintsLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  verticalLayout->addWidget(hintsLabel);
}

// --------------------------------------------------------------------------
QString voView::hints()const
{
  return QString();
}

// --------------------------------------------------------------------------
voDataObject* voView::dataObject()const
{
  Q_D(const voView);
  return d->DataObject;
}

// --------------------------------------------------------------------------
void voView::setDataObject(voDataObject* dataObject)
{
  Q_D(voView);
  if (!dataObject)
    {
    qCritical() << qPrintable(this->objectName())
                << "- Failed to setDataObject - dataObject is NULL";
    return;
    }
  d->DataObject = dataObject;
  this->setDataObjectInternal(dataObject);
}
