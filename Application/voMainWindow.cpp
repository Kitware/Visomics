
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
#include "ui_voMainWindow.h"
#include "voAnalysisParameterDialog.h"
#include "voApplication.h"
#include "voAnalysis.h"
#include "voAnalysisDriver.h"
#include "voAnalysisFactory.h"
#include "voApplication.h"
#include "voDataModel.h"
#include "voIOManager.h"
#include "voMainWindow.h"
#include "voViewManager.h"
#include "voViewStackedWidget.h"

// --------------------------------------------------------------------------
class voMainWindowPrivate: public Ui_voMainWindow
{
  Q_DECLARE_PUBLIC(voMainWindow);
protected:
  voMainWindow* const q_ptr;
public:
  voMainWindowPrivate(voMainWindow& object);

  voViewStackedWidget* ViewStackedWidget;
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

  d->ViewStackedWidget = new voViewStackedWidget(this);
  this->setCentralWidget(d->ViewStackedWidget);

  connect(voApplication::application()->viewManager(),
          SIGNAL(viewCreated(const QString&, voView*)),
          d->ViewStackedWidget,
          SLOT(addView(const QString&, voView*)));

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

  voDataModel * dataModel = voApplication::application()->dataModel();

  // Set data model
  d->DataBrowserWidget->setModel(dataModel);

  // Set selection model
  d->DataBrowserWidget->setSelectionModel(dataModel->selectionModel());

  // By default, hide the dock widget
  d->AnalysisParameterDockWidget->setVisible(false);

  connect(dataModel, SIGNAL(analysisSelected(voAnalysis*)),
          SLOT(onAnalysisSelected(voAnalysis*)));

  connect(dataModel, SIGNAL(activeAnalysisChanged(voAnalysis*)),
          SLOT(onActiveAnalysisChanged(voAnalysis*)));

  connect(voApplication::application()->analysisDriver(),
          SIGNAL(aboutToRunAnalysis(voAnalysis*)),
          SLOT(onAboutToRunAnalysis(voAnalysis*)));

  connect(d->AnalysisParameterEditorWidget,
          SIGNAL(runAnalysisRequested(const QString&, const QHash<QString, QVariant>&)),
          voApplication::application()->analysisDriver(),
          SLOT(runAnalysisForCurrentInput(const QString&, const QHash<QString, QVariant>&)));

  connect(d->AnalysisParameterEditorWidget,
          SIGNAL(updateAnalysisRequested(voAnalysis*, const QHash<QString, QVariant>&)),
          voApplication::application()->analysisDriver(),
          SLOT(updateAnalysis(voAnalysis*, const QHash<QString, QVariant>&)));

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

// --------------------------------------------------------------------------
void voMainWindow::onAnalysisSelected(voAnalysis* analysis)
{
  Q_UNUSED(analysis);
  // Q_D(voMainWindow);
}

// --------------------------------------------------------------------------
void voMainWindow::onActiveAnalysisChanged(voAnalysis* analysis)
{
  Q_D(voMainWindow);
  d->AnalysisParameterDockWidget->setVisible(analysis != 0 && analysis->parameterCount() > 0);
  d->AnalysisParameterEditorWidget->setAnalysis(analysis);
}

// --------------------------------------------------------------------------
void voMainWindow::onAboutToRunAnalysis(voAnalysis* analysis)
{
  // Show parameter dialog if needed
  if (!analysis->acceptDefaultParameterValues() && analysis->parameterCount())
    {
    voAnalysisParameterDialog analysisParameterDialog(analysis, this);
    if (analysisParameterDialog.exec() != QDialog::Accepted)
      {
      return;
      }
    }
}
