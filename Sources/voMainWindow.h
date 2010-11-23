

// Qt includes
#include <QScopedPointer>
#include <QMainWindow>
#include <QMap>

class QDockWidget;
class QSplitter;
class QTreeWidget;
class QTableWidget;

class voAnalysisPanel;
class voCSVReader;
class voAnalysis;

class voMainWindowPrivate;

class voMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  typedef QMainWindow Superclass;
  voMainWindow(QWidget * newParent = 0);
  virtual ~voMainWindow();
  
  typedef enum { RAW, PCA } AnalysisMode;
  
public slots:
  void open();

  void about();

  void pca();

  void selectItemTreeWidget();

  void loadRawData();
  
private:

  bool loadFile ( const QString  &filename);

  void setCurrentFile ( const QString &filename );

  QString strippedName( const QString &fullFileName );

  void addData( const QString &filename);

  AnalysisMode  mode;

protected:
  QScopedPointer<voMainWindowPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voMainWindow);
  Q_DISABLE_COPY(voMainWindow);
};
