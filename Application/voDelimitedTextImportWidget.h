
#ifndef __voDelimitedTextImportWidget_h
#define __voDelimitedTextImportWidget_h

// Qt includes
#include <QWidget>

class voDelimitedTextPreviewModel;
class voDelimitedTextImportWidgetPrivate;

class voDelimitedTextImportWidget : public QWidget
{
  Q_OBJECT
public:
  typedef voDelimitedTextImportWidget Self;
  typedef QWidget Superclass;

  voDelimitedTextImportWidget(QWidget* newParent = 0);
  virtual ~voDelimitedTextImportWidget();

  enum InsertWidgetLocation
    {
    DelimiterGroupBox = 0,
    RowsAndColumnsGroupBox,
    };

  void insertWidget(QWidget * widget, InsertWidgetLocation location);

  voDelimitedTextPreviewModel* delimitedTextPreviewModel();
  void setDelimitedTextPreviewModel(voDelimitedTextPreviewModel* model);

public slots:

  void setFileName(const QString& fileName);

protected slots:
  void onNumberOfColumnMetaDataTypesChanged(int value);
  void onColumnMetaDataTypeOfInterestChanged(int value);
  void onNumberOfRowMetaDataTypesChanged(int value);
  void onRowMetaDataTypeOfInterestChanged(int value);

  void onDelimiterChanged(int delimiter);
  void onOtherDelimiterLineEditChanged(const QString& text);

  void onStringDelimiterEnabled(bool value);
  void onStringDelimiterLineEditChanged(const QString& text);

protected:
  QScopedPointer<voDelimitedTextImportWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDelimitedTextImportWidget);
  Q_DISABLE_COPY(voDelimitedTextImportWidget);
};

#endif

