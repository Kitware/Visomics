
// Qt includes
#include <QVBoxLayout>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkDataObject.h>

// --------------------------------------------------------------------------
voView::voView(QWidget* newParent):Superclass(newParent)
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
}
