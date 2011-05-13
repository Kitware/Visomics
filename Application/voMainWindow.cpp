
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
#include "voDelimitedTextImportDialog.h"
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
  bool AnalysisParametersPrevShown; // Remembers previous user-selected state of widget
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
  this->setWindowTitle(QString("Visomics ") + QString(Visomics_VERSION_STRING));

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
  foreach(const QString& analysisName, analysisFactory->registeredAnalysisPrettyNames())
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
  // ... and disable the associated action
  d->actionViewAnalysisParameters->setEnabled(false);
  // ... but ensure it is shown when it first becomes available
  d->AnalysisParametersPrevShown = true;

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

  connect(voApplication::application()->analysisDriver(),
          SIGNAL(analysisAddedToObjectModel(voAnalysis*)),
          d->DataBrowserWidget,
          SLOT(setActiveAnalysis(voAnalysis*)));

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

  voDelimitedTextImportDialog dialog(this);

  foreach(const QString& file, files)
    {
    dialog.setFileName(file);
    int status = dialog.exec();
    if (status == voDelimitedTextImportDialog::Accepted)
      {
      voApplication::application()->ioManager()->openCSVFile(file, dialog.importSettings());
      }
    }
}

// --------------------------------------------------------------------------
void voMainWindow::about()
{
  QMessageBox::about(this, tr("About Visomics"),
          tr("<h2>Visomics %1</h2>"
             "<p>Copyright &copy; 2010 Kitware Inc."
             "<p>Visomics is a platform for visualization and analysis of omiccs data.").arg(Visomics_VERSION_STRING));
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
  bool showAnalysisParameterDock = analysis != 0 && analysis->parameterCount() > 0;

  if(!showAnalysisParameterDock && d->actionViewAnalysisParameters->isEnabled())
    {
    // If widget is going to disable and is currently enabled, remember previous visible state
    d->AnalysisParametersPrevShown = d->AnalysisParameterDockWidget->isVisible();
    d->AnalysisParameterDockWidget->setVisible(false);
    }
  else if(showAnalysisParameterDock && !d->actionViewAnalysisParameters->isEnabled())
    {
    // If widget is going to enable and is currently disabled, recall previous visible state
    d->AnalysisParameterDockWidget->setVisible(d->AnalysisParametersPrevShown);
    }
  d->actionViewAnalysisParameters->setEnabled(showAnalysisParameterDock);
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
      analysis->setAbortExecution(true);
      return;
      }
    }
}
