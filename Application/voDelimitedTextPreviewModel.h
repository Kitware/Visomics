
#ifndef __voDelimitedTextPreviewModel_h
#define __voDelimitedTextPreviewModel_h

// Qt includes
#include <QStandardItemModel>
#include <QString>

// Macros - TODO: find a way to import and include ctkPimpl.h

// Defines
#define NUM_FILE_LINES_READ 100


class voDelimitedTextPreviewModelPrivate;

class voDelimitedTextPreviewModel : public QStandardItemModel
{
  Q_OBJECT

public:
  typedef QStandardItemModel Superclass;
  voDelimitedTextPreviewModel(const QString& newFileName, QObject* newParent = 0);
  virtual ~voDelimitedTextPreviewModel();

  QString getFileName() const;
  void setFileName(const QString& _arg);

  char getFieldDelimiter() const;
  void setFieldDelimiter (char _arg);

  char getStringBeginEndCharacter() const;
  void setStringBeginEndCharacter(char _arg); // Value of 0 indicates none

  bool getUseFirstLineAsAttributeNames() const;
  void setUseFirstLineAsAttributeNames(bool _arg);

  bool getTranspose() const;
  void setTranspose(bool _arg);

  int getHeaderColumnNumber() const;
  void setHeaderColumnNumber(int _arg);

  int getHeaderRowNumber() const;
  void setHeaderRowNumber(int _arg);

  int getPreviewColumnNumber() const; // Number of columns to skip
  void setPreviewColumnNumber(int _arg);

  int getPreviewRowNumber() const; // Number of rows to skip
  void setPreviewRowNumber(int _arg);

  bool getInlineUpdate() const;
  void setInlineUpdate(bool _arg);



public slots:
  void updatePreview();

private:
  QScopedPointer<voDelimitedTextPreviewModelPrivate> d_ptr;

  Q_DECLARE_PRIVATE(voDelimitedTextPreviewModel);
  Q_DISABLE_COPY(voDelimitedTextPreviewModel);
};

#endif

