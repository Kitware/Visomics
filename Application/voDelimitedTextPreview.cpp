
// Visomics includes
#include "voDelimitedTextPreview.h"
//#include "ui_voDelimitedTextPreview.h"

class voDelimitedTextPreviewPrivate //: public Ui_voDelimitedTextPreview
{
  Q_DECLARE_PUBLIC(voDelimitedTextPreview);

private:
  voDelimitedTextPreview* const q_ptr;

public:
  voDelimitedTextPreviewPrivate();

  QString fileName;
  char fieldDelimiter;
  char stringBeginEndCharacter;
  bool useFirstLineAsAttributeNames;
  bool transpose;
  int headerColumnNumber;
  int headerRowNumber;
  int previewColumnNumber;
  int previewRowNumber;
  bool inlineUpdate;
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
  Q_D(voDelimitedTextPreview);
//  d->setupUi(this);
}

// --------------------------------------------------------------------------
voDelimitedTextPreview::~voDelimitedTextPreview()
{
}

