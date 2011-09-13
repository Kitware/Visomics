
// Qt includes
#include <QAction>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QSignalMapper>
#include <QToolBar>

// CTK includes
#include <ctkErrorLogWidget.h>

// Visomics includes
#include "ui_voMainWindow.h"
#include "voAnalysisParameterDialog.h"
#include "voApplication.h"
#include "voAnalysis.h"
#include "voAnalysisDriver.h"
#include "voAnalysisFactory.h"
#include "voApplication.h"
#include "voConfigure.h" // For Visomics_INSTALL_DATA_DIR, Visomics_VERSION, Visomics_BUILD_TESTING
#include "voDataModel.h"
#include "voDelimitedTextImportDialog.h"
#include "voIOManager.h"
#include "voMainWindow.h"
#ifdef Visomics_BUILD_TESTING
# include "voTestConfigure.h"
#endif
#include "voView.h"
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

  ctkErrorLogWidget  ErrorLogWidget;
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
  this->setWindowTitle(QString("Visomics %1").arg(Visomics_VERSION));

  d->ErrorLogWidget.setErrorLogModel(voApplication::application()->errorLogModel());

  d->ViewStackedWidget = new voViewStackedWidget(this);
  this->setCentralWidget(d->ViewStackedWidget);

  connect(voApplication::application()->viewManager(),
          SIGNAL(viewCreated(const QString&, voView*)),
          d->ViewStackedWidget,
          SLOT(addView(const QString&, voView*)));

  connect(voApplication::application()->viewManager(),
          SIGNAL(viewCreated(const QString&, voView*)),
          SLOT(setViewActions(const QString&, voView*)));

  // Setup actions
  d->actionFileOpen->setShortcut(QKeySequence::Open);
  connect(d->actionFileOpen, SIGNAL(triggered()), this, SLOT(onFileOpenActionTriggered()));
  connect(d->actionFileExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(d->actionHelpAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(d->actionLoadSampleDataset, SIGNAL(triggered()), this, SLOT(loadSampleDataset()));
  connect(d->actionViewErrorLog, SIGNAL(triggered()), this, SLOT(onViewErrorLogActionTriggered()));

  // Populate Analysis menu
  voAnalysisFactory* analysisFactory = voApplication::application()->analysisFactory();
  QList<QAction*> analysisActions;
  QStringList registeredAnalysisPrettyNames = analysisFactory->registeredAnalysisPrettyNames();
  registeredAnalysisPrettyNames.sort();
  foreach(const QString& analysisName, registeredAnalysisPrettyNames)
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

//-----------------------------------------------------------------------------
void voMainWindow::onViewErrorLogActionTriggered()
{
  Q_D(voMainWindow);

  bool wasVisible = d->ErrorLogWidget.isVisible();
  d->ErrorLogWidget.show();

  // Center dialog if wasn't visible
  if (!wasVisible)
    {
    QRect screen = QApplication::desktop()->screenGeometry(this);
    d->ErrorLogWidget.move(screen.center() - d->ErrorLogWidget.rect().center());
    }

  d->ErrorLogWidget.activateWindow();
  d->ErrorLogWidget.raise();
}

// --------------------------------------------------------------------------
void voMainWindow::about()
{
  QMessageBox::about(this, tr("About Visomics"),
          tr("<h2>Visomics %1</h2>"
             "<p>Copyright &copy; 2010 Kitware Inc."
             "<p>Visomics is a platform for visualization and analysis of 'omics data.").arg(Visomics_VERSION));
}

// --------------------------------------------------------------------------
void voMainWindow::loadSampleDataset()
{
#ifdef Visomics_BUILD_TESTING
  voApplication * app = voApplication::application();
  QString file = app->homeDirectory() + "/" + Visomics_INSTALL_DATA_DIR + "/All_conc_kitware_transposed.csv";
  if (!app->isInstalled())
    {
    file = QString(VISOMICS_DATA_DIR) + "/Data/UNC/All_conc_kitware_transposed.csv";
    }
  voDelimitedTextImportSettings defaultSettings;
  defaultSettings.insert(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes, 4);
  voDelimitedTextImportDialog dialog(this, defaultSettings);
  dialog.setFileName(file);
  int status = dialog.exec();
  if (status == voDelimitedTextImportDialog::Accepted)
    {
    voApplication::application()->ioManager()->openCSVFile(file, dialog.importSettings());
    }
#else
  qWarning() << "Sample dataset not available !";
#endif
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

// --------------------------------------------------------------------------
void voMainWindow::setViewActions(const QString& objectUuid, voView* newView)
{
  Q_D(voMainWindow);
  Q_UNUSED(objectUuid);
  d->ViewActionsToolBar->clear();
  d->ViewActionsToolBar->addActions(newView->actions());
}
