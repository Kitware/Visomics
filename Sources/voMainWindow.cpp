
// Qt includes
#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QDebug>
#include <QSignalMapper>

// Visomics includes
#include "voMainWindow.h"
#include "ui_voMainWindow.h"
#include "voApplication.h"
#include "voAnalysisDriver.h"
#include "voAnalysisFactory.h"
#include "voApplication.h"
#include "voDataModel.h"
#include "voIOManager.h"
#include "voViewManager.h"
#include "voViewTabWidget.h"

// --------------------------------------------------------------------------
class voMainWindowPrivate: public Ui_voMainWindow
{
  Q_DECLARE_PUBLIC(voMainWindow);
protected:
  voMainWindow* const q_ptr;
public:
  voMainWindowPrivate(voMainWindow& object);

  voViewTabWidget *  ViewTabWidget;
  QSignalMapper      AnalysisActionMapper;
};

// --------------------------------------------------------------------------
// voMainWindowPrivate methods

// --------------------------------------------------------------------------
voMainWindowPrivate::voMainWindowPrivate(voMainWindow &object) : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
// voMainWindow methods

// --------------------------------------------------------------------------
voMainWindow::voMainWindow(QWidget * newParent)
    : Superclass(newParent)
    , d_ptr(new voMainWindowPrivate(*this))
{
  Q_D(voMainWindow);

  d->setupUi(this);

  d->ViewTabWidget = new voViewTabWidget(this);
  this->setCentralWidget(d->ViewTabWidget);

  connect(voApplication::application()->viewManager(),
          SIGNAL(viewCreated(const QString&, voView*)),
          d->ViewTabWidget,
          SLOT(addViewTab(const QString&, voView*)));

  // Setup actions
  d->actionFileOpen->setShortcut(QKeySequence::Open);
  connect(d->actionFileOpen, SIGNAL(triggered()), this, SLOT(onFileOpenActionTriggered()));
  connect(d->actionFileExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(d->actionHelpAbout, SIGNAL(triggered()), this, SLOT(about()));

  // Populate Analysis menu
  voAnalysisFactory* analysisFactory = voApplication::application()->analysisFactory();
  QList<QAction*> analysisActions;
  foreach(const QString& analysisName, analysisFactory->registeredAnalysisNames())
    {
    QAction * action = new QAction(analysisName, this);
    d->AnalysisActionMapper.setMapping(action, analysisName);
    connect(action, SIGNAL(triggered()), &d->AnalysisActionMapper, SLOT(map()));
    analysisActions << action;
    }
  d->menuAnalysis->addActions(analysisActions);

  connect(&d->AnalysisActionMapper, SIGNAL(mapped(QString)),
          voApplication::application()->analysisDriver(), SLOT(runAnalysisForAllInputs(QString)));

  // Set data model
  d->DataBrowserWidget->setModel(voApplication::application()->dataModel());

  // Set selection model
  d->DataBrowserWidget->setSelectionModel(
      voApplication::application()->dataModel()->selectionModel());

  // Initialize status bar
  this->statusBar()->showMessage(tr(""), 2000);
}

// --------------------------------------------------------------------------
voMainWindow::~voMainWindow()
{
}

// --------------------------------------------------------------------------
void voMainWindow::onFileOpenActionTriggered()
{
  QStringList files = QFileDialog::getOpenFileNames(
      this, tr("Open table data"), "*.csv", tr("CSV files (*.csv)"));

  files.sort();

  voApplication::application()->ioManager()->openCSVFiles(files);
}

// --------------------------------------------------------------------------
void voMainWindow::about()
{
  QMessageBox::about(this, tr("About OmicsView"),
          tr("<h2>OmicsView 1.1</h2>"
             "<p>Copyright &copy; 2010 Kitware Inc."
             "<p>Omicsview is a platform for visualization and analysis of omiccs data."));
}
