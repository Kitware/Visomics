#ifndef __voDelimitedTextImportDialog_h
#define __voDelimitedTextImportDialog_h

// Qt includes
#include <QDialog>

// Visomics includes
#include "voDelimitedTextImportSettings.h"

class voDelimitedTextImportDialogPrivate;

class voDelimitedTextImportDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;

  voDelimitedTextImportDialog(QWidget* newParent = 0);
  virtual ~voDelimitedTextImportDialog();

  void setFileName(const QString& fileName);

  voDelimitedTextImportSettings importSettings()const;

protected:
  QScopedPointer<voDelimitedTextImportDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDelimitedTextImportDialog);
  Q_DISABLE_COPY(voDelimitedTextImportDialog);
};

#endif

