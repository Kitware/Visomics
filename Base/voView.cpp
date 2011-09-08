
// Qt includes
#include <QLabel>
#include <QVBoxLayout>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkDataObject.h>

//----------------------------------------------------------------------------
class voViewPrivate
{
public:
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

