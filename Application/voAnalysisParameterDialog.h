
#ifndef __voAnalysisParameterDialog_h
#define __voAnalysisParameterDialog_h

// Qt includes
#include <QDialog>

class voAnalysis;
class voAnalysisParameterDialogPrivate;

class voAnalysisParameterDialog : public QDialog
{
  Q_OBJECT
public:
  typedef QDialog Superclass;
  voAnalysisParameterDialog(voAnalysis * analysis, QWidget* newParent = 0);
  virtual ~voAnalysisParameterDialog();

protected:
  QScopedPointer<voAnalysisParameterDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisParameterDialog);
  Q_DISABLE_COPY(voAnalysisParameterDialog);
};

#endif
