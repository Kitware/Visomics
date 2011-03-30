
#ifndef __voDelimitedTextPreview_h
#define __voDelimitedTextPreview_h

// Qt includes
//#include <QWidget>

//Macros - adapted from vtkSetGet.h
#define class_name voDelimitedTextPreview

#define pimplInlineUpdateSetMacro(name,type) \
void Set##name (type _arg) \
  { \
  Q_D(class_name); \
  if (d->name != _arg) \
    { \
    d->name = _arg; \
    if (d->inlineUpdate) \
      { \
      this->updatePreview(); \
      } \
    } \
  } 

#define pimplGetMacro(name,type) \
type Get##name () const \
  { \
  Q_D(class_name); \
  return d->name; \
  } 

class voDelimitedTextPreviewPrivate;

class voDelimitedTextPreview : public QWidget
{
  Q_OBJECT

public:
  typedef QWidget Superclass;
  voDelimitedTextPreview(QWidget* newParent = 0);
  virtual ~voDelimitedTextPreview();

  pimplGetMacro(fileName, QString);
  pimplInlineUpdateSetMacro(fileName, QString);

  pimplGetMacro(fieldDelimiter, char);
  pimplInlineUpdateSetMacro(fieldDelimiter, char);

  pimplGetMacro(stringBeginEndCharacter, char);
  pimplInlineUpdateSetMacro(stringBeginEndCharacter, char);

  pimplGetMacro(useFirstLineAsAttributeNames, bool);
  pimplInlineUpdateSetMacro(useFirstLineAsAttributeNames, bool);

  pimplGetMacro(transpose, bool);
  pimplInlineUpdateSetMacro(transpose, bool);

  pimplGetMacro(headerColumnNumber, int);
  pimplInlineUpdateSetMacro(headerColumnNumber, int);

  pimplGetMacro(headerRowNumber, int);
  pimplInlineUpdateSetMacro(headerRowNumber, int);

  pimplGetMacro(previewColumnNumber, int);
  pimplInlineUpdateSetMacro(previewColumnNumber, int);

  pimplGetMacro(previewRowNumber, int);
  pimplInlineUpdateSetMacro(previewRowNumber, int);

  pimplGetMacro(inlineUpdate, bool);
  pimplInlineUpdateSetMacro(inlineUpdate, bool);

public slots:
  void updatePreview();

private:
  QScopedPointer<voDelimitedTextPreviewPrivate> d_ptr;

  Q_DECLARE_PRIVATE(voDelimitedTextPreview);
  Q_DISABLE_COPY(voDelimitedTextPreview);
};

#endif

