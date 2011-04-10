
#ifndef __voDelimitedTextPreviewModel_h
#define __voDelimitedTextPreviewModel_h

// Qt includes
#include <QStandardItemModel>
#include <QString>

// Macros - TODO: find a way to import and include ctkPimpl.h

class voDelimitedTextPreviewModelPrivate;

class voDelimitedTextPreviewModel : public QStandardItemModel
{
  Q_OBJECT

public:
  typedef QStandardItemModel Superclass;
  voDelimitedTextPreviewModel(QObject* newParent = 0);
  virtual ~voDelimitedTextPreviewModel();

  QString fileName() const;
  void setFileName(const QString& newFileName);

  char fieldDelimiter() const;
  void setFieldDelimiter (char _arg);

  char stringBeginEndCharacter() const;
  void setStringBeginEndCharacter(char _arg); // Value of 0 indicates none

  bool useFirstLineAsAttributeNames() const;
  void setUseFirstLineAsAttributeNames(bool _arg);

  bool transpose() const;
  void setTranspose(bool _arg);

  int headerColumnNumber() const;
  void setHeaderColumnNumber(int _arg);

  int headerRowNumber() const;
  void setHeaderRowNumber(int _arg);

  int numberOfRowsToPreview() const;
  void setNumberOfRowsToPreview(int count);

  bool inlineUpdate() const;
  void setInlineUpdate(bool _arg);



public slots:
  void updatePreview();

private:
  QScopedPointer<voDelimitedTextPreviewModelPrivate> d_ptr;

  Q_DECLARE_PRIVATE(voDelimitedTextPreviewModel);
  Q_DISABLE_COPY(voDelimitedTextPreviewModel);
};

#endif

