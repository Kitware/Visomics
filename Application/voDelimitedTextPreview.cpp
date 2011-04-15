
// Visomics includes
#include "voDelimitedTextPreview.h"

class voDelimitedTextPreviewPrivate
{
  Q_DECLARE_PUBLIC(voDelimitedTextPreview);

private:
  voDelimitedTextPreview* const q_ptr;

public:
  voDelimitedTextPreviewPrivate(voDelimitedTextPreview& object);
};

// --------------------------------------------------------------------------
// voDelimitedTextPreviewPrivate methods

// --------------------------------------------------------------------------
voDelimitedTextPreviewPrivate::voDelimitedTextPreviewPrivate(voDelimitedTextPreview& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
// voDelimitedTextPreview methods

// --------------------------------------------------------------------------
voDelimitedTextPreview::voDelimitedTextPreview(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voDelimitedTextPreviewPrivate(*this))
{
//  Q_D(voDelimitedTextPreview);
}

// --------------------------------------------------------------------------
voDelimitedTextPreview::~voDelimitedTextPreview()
{
}

