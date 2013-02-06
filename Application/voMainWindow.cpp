
// Qt includes
#include <QAbstractButton>
#include <QAction>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QItemSelection>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QSignalMapper>
#include <QToolBar>

// CTK includes
#include <ctkErrorLogWidget.h>

// Visomics includes

#include "voConfigure.h" // For Visomics_INSTALL_DATA_DIR, Visomics_VERSION, Visomics_BUILD_TESTING, USE_ARBOR_BRAND
#ifdef USE_ARBOR_BRAND
  #include "ui_voMainWindow_Arbor.h"
  #include "voAboutDialog_Arbor.h"
#else
  #include "ui_voMainWindow.h"
  #include "voAboutDialog.h"
#endif

#include "voAnalysisParameterDialog.h"
#include "voApplication.h"
#include "voAnalysis.h"
#include "voAnalysisDriver.h"
#include "voAnalysisFactory.h"
#include "voApplication.h"
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voDelimitedTextImportDialog.h"
#include "voInputFileDataObject.h"
#include "voIOManager.h"
#include "voMainWindow.h"
#include "voStartupView.h"
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
 
#ifdef USE_ARBOR_BRAND
  this->setWindowTitle(QString("Arbor %1").arg(Visomics_VERSION));
#else
  this->setWindowTitle(QString("Visomics %1").arg(Visomics_VERSION));
#endif

  d->ErrorLogWidget.setErrorLogModel(voApplication::application()->errorLogModel());

  d->ViewStackedWidget = new voViewStackedWidget(this);
  this->setCentralWidget(d->ViewStackedWidget);

  d->ViewStackedWidget->addWidget(new voStartupView(this));

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
  d->actionFileClose->setShortcut(QKeySequence::Close);
  connect(d->actionFileClose, SIGNAL(triggered()), this, SLOT(onCloseActionTriggered()));
  connect(d->actionFileExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(d->actionHelpAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(d->actionLoadSampleDataset, SIGNAL(triggered()), this, SLOT(loadSampleDataset()));
  connect(d->actionViewErrorLog, SIGNAL(triggered()), this, SLOT(onViewErrorLogActionTriggered()));
  connect(d->actionFileSaveState, SIGNAL(triggered()), this, SLOT(onFileSaveStateActionTriggered()));
  connect(d->actionFileLoadState, SIGNAL(triggered()), this, SLOT(onFileLoadStateActionTriggered()));
  connect(d->actionFileMakeTreeHeatmap, SIGNAL(triggered()),
          this, SLOT(onFileMakeTreeHeatmapActionTriggered()));

  // Populate Analysis menu
  voAnalysisFactory* analysisFactory = voApplication::application()->analysisFactory();
  QList<QAction*> analysisActions;
  QStringList registeredAnalysisPrettyNames = analysisFactory->registeredAnalysisPrettyNames();
  registeredAnalysisPrettyNames.sort();
  foreach(const QString& analysisName, registeredAnalysisPrettyNames)
    {
    QAction * action = new QAction(analysisName, this);
    action->setEnabled(false);
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

  connect(dataModel, SIGNAL(inputSelected(voDataModelItem*)), this,
          SLOT(onInputSelected(voDataModelItem*)));

  connect(dataModel,
          SIGNAL(objectRemoved(const QString&)),
          voApplication::application()->viewManager(),
          SLOT(deleteView(const QString&)));

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

  connect(voApplication::application()->analysisDriver(),
          SIGNAL(addedCustomAnalysis(const QString&)),
          this,
          SLOT(onCustomAnalysisAdded(const QString&)));

  this->loadAnalysisScripts();

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
    this, tr("Open table or tree"), "", tr("All files(*.*);;*.csv(*.csv);;*.phy(*.phy);;*.tre(*.tre)"));

  files.sort();

  QStringList acceptedImageFileTypeList;
  acceptedImageFileTypeList << "csv" << "phy" << "tre" << "xml";

  foreach(const QString& file, files)
    {
    QStringList splittedStrList = file.split(".");

    if( ((splittedStrList.size())-1) >= 0)
      {
      QString extension = splittedStrList[splittedStrList.size()-1];
      if (acceptedImageFileTypeList.contains(extension))
        {
        if ( extension  == "csv" )
          {
          voDelimitedTextImportDialog dialog(this);
          dialog.setFileName(file);
          int status = dialog.exec();
          if (status == voDelimitedTextImportDialog::Accepted)
            {
            voApplication::application()->ioManager()->openCSVFile(file, dialog.importSettings());
            }
          }
        if (extension == "phy" || extension == "tre")
          {
            //open associated tree node data
            QString fileExt = "";
            QMessageBox msgBox;
            msgBox.setInformativeText("Do you want to load associated data (*.csv) with the tree file?");
            msgBox.setStandardButtons(QMessageBox::Yes  |  QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Yes)
              {
              QString fileExt = QFileDialog::getOpenFileName(
                this, tr("Open associated tree node table"), "", tr("*.csv(*.csv)"));
              voDelimitedTextImportDialog dialog(this);
              dialog.setFileName(fileExt);
              int status = dialog.exec();
              if (status == voDelimitedTextImportDialog::Accepted)
                {
                voApplication::application()->ioManager()->loadPhyloTreeDataSet(file, fileExt, dialog.importSettings());
                }
              else
                {
                QMessageBox errorMsgBox;
                errorMsgBox.setInformativeText("Failed to load the table data.");
                errorMsgBox.exec();
                voApplication::application()->ioManager()->loadPhyloTreeDataSet(file);
                }
              }
            else
              {
              voApplication::application()->ioManager()->loadPhyloTreeDataSet(file);
              }
          }
        if (extension == "xml")
          {
          voApplication::application()->ioManager()->loadState(file);
          }
        }
      else
        {
        qWarning()<<"This is an invalid input file.";
        }
      }
    }
}

// --------------------------------------------------------------------------
void voMainWindow::onFileMakeTreeHeatmapActionTriggered()
{
  Q_D(voMainWindow);

  // temporarily enable multi-select
  d->DataBrowserWidget->setSelectionMode(QAbstractItemView::MultiSelection);

  // display a non-modal dialog
  QMessageBox* msgBox = new QMessageBox();
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setInformativeText("Select the tree and the table that you'd like to combine.");
  msgBox->setStandardButtons(QMessageBox::Ok  |  QMessageBox::Cancel);
  msgBox->setDefaultButton(QMessageBox::Cancel);
  msgBox->setModal(false);
  msgBox->setWindowModality(Qt::NonModal);

  // workaround Qt bug where accepted() and rejected() aren't emitted when
  // the user clicks on "Ok" or "Cancel", respectively.
  connect(msgBox->buttons()[0], SIGNAL(clicked()),
          this, SLOT(makeTreeHeatmap()));
  connect(msgBox->buttons()[1], SIGNAL(clicked()),
          this, SLOT(makeTreeHeatmapDialogClosed()));
  // rejected() IS emitted when the user closes the dialog, however.
  connect(msgBox, SIGNAL(rejected()),
          this, SLOT(makeTreeHeatmapDialogClosed()));

  msgBox->show();
}

// --------------------------------------------------------------------------
void voMainWindow::makeTreeHeatmap()
{
  // make sure exactly two items are selected
  voDataModel * dataModel = voApplication::application()->dataModel();
  QItemSelection selection = dataModel->selectionModel()->selection();
  if (selection.size() != 2)
    {
    qCritical() << "Exactly two inputs must be selected";
    this->makeTreeHeatmapDialogClosed();
    return;
    }

  // make sure one is a tree and the other is a table
  voDataModelItem * firstItem = dynamic_cast<voDataModelItem*>
    (dataModel->itemFromIndex(selection.indexes()[0]));
  voDataModelItem * secondItem = dynamic_cast<voDataModelItem*>
    (dataModel->itemFromIndex(selection.indexes()[1]));

  voInputFileDataObject* treeObject = NULL;
  voInputFileDataObject* tableObject = NULL;

  // tree is 1st, table is 2nd
  if (firstItem->dataObject()->type() == "vtkTree")
    {
    treeObject = dynamic_cast<voInputFileDataObject*>(firstItem->dataObject());
    if (!treeObject)
      {
      qCritical() << "Expected vtkTree, found NULL";
      this->makeTreeHeatmapDialogClosed();
      return;
      }

    tableObject = dynamic_cast<voInputFileDataObject*>(secondItem->dataObject());
    if (!tableObject)
      {
      qCritical() << "Expected vtkExtendedTable, found NULL";
      this->makeTreeHeatmapDialogClosed();
      return;
      }

    QString type = tableObject->type();
    if (type != "vtkExtendedTable")
      {
      qCritical() << QString("Expected vtkExtendedTable, found %1").arg(type);
      this->makeTreeHeatmapDialogClosed();
      return;
      }
    }

  // table is 1st, tree is 2nd
  else if (firstItem->dataObject()->type() == "vtkExtendedTable")
    {
    tableObject = dynamic_cast<voInputFileDataObject*>(firstItem->dataObject());
    if (!tableObject)
      {
      qCritical() << "Expected vtkExtendedTable, found NULL";
      this->makeTreeHeatmapDialogClosed();
      return;
      }

    treeObject =
      dynamic_cast<voInputFileDataObject*>(secondItem->dataObject());
    if (!treeObject)
      {
      qCritical() << "Expected vtkTree, found NULL";
      this->makeTreeHeatmapDialogClosed();
      return;
      }

    QString type = treeObject->type();
    if (type != "vtkTree")
      {
      qCritical() << QString("Expected vtkTree, found %1").arg(type);
      this->makeTreeHeatmapDialogClosed();
      return;
      }
    }
  else
    {
    qCritical() << QString("Expected vtkTree or vtkExtendedTable, found %1").
        arg(firstItem->dataObject()->type());
    }

  // create the new grouped entry and remove the old items
  QStandardItem *firstParent =
    dataModel->itemFromIndex(dataModel->parent(dataModel->indexFromItem(firstItem)));
  QStandardItem *secondParent =
    dataModel->itemFromIndex(dataModel->parent(dataModel->indexFromItem(secondItem)));

  QString treeHeatmapName =
    QString("%1 TreeHeatmap").arg(QFileInfo(treeObject->fileName()).baseName());
  voApplication::application()->ioManager()->createTreeHeatmapItem(
    treeHeatmapName, treeObject, tableObject);

  dataModel->removeObject(firstItem, firstParent);
  dataModel->removeObject(secondItem, secondParent);

  this->makeTreeHeatmapDialogClosed();
}

// --------------------------------------------------------------------------
void voMainWindow::makeTreeHeatmapDialogClosed()
{
  Q_D(voMainWindow);

  // clear the selection
  d->DataBrowserWidget->clearSelection();

  // revert selection mode back to normal afterwards
  d->DataBrowserWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

//-----------------------------------------------------------------------------
void voMainWindow::onCloseActionTriggered()
{
  voDataModelItem* objectToRemove =
    voApplication::application()->dataModel()->selectedObject();

  voApplication::application()->dataModel()->removeObject(objectToRemove);
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
void voMainWindow::onFileSaveStateActionTriggered()
{
  QString fileName =
    QFileDialog::getSaveFileName(this, tr("Save State"), "",
                                 tr(".xml file (*.xml)"));
  voApplication::application()->ioManager()->saveState(fileName);
}

// --------------------------------------------------------------------------
void voMainWindow::onFileLoadStateActionTriggered()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, tr("Save State"), "",
                                 tr(".xml file (*.xml)"));
  voApplication::application()->ioManager()->loadState(fileName);
}

// --------------------------------------------------------------------------
void voMainWindow::about()
{
  voAboutDialog aboutDialog(this);
  aboutDialog.exec();
}

// --------------------------------------------------------------------------
void voMainWindow::loadSampleDataset()
{
#ifdef Visomics_BUILD_TESTING
  voApplication * app = voApplication::application();
  QString file = app->homeDirectory() + "/" + Visomics_INSTALL_DATA_DIR + "/visomics-sample_liver.csv";
  if (!app->isInstalled())
    {
    file = QString(VISOMICS_DATA_DIR) + "/Data/sample/visomics-sample_liver.csv";
    }
  voDelimitedTextImportSettings defaultSettings;
  defaultSettings.insert(voDelimitedTextImportSettings::NumberOfColumnMetaDataTypes, 3);
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
void voMainWindow::onInputSelected(voDataModelItem* inputTarget)
{
  Q_D(voMainWindow);

  voAnalysisDriver *driver = voApplication::application()->analysisDriver();

  for (int i = 0; i < d->menuAnalysis->actions().size(); ++i)
    {
    QAction *analysisAction = d->menuAnalysis->actions().at(i);
    if (driver->doesInputMatchAnalysis(analysisAction->text(), inputTarget,
                                       false))
      {
      analysisAction->setEnabled(true);
      }
    else
      {
      analysisAction->setEnabled(false);
      }
    if (inputTarget->childItems().size() > 0)
      {
      for (int i = 0; i < inputTarget->childItems().size(); ++i)
        {
        voDataModelItem *childItemForSingleInput =
          dynamic_cast<voDataModelItem*>(inputTarget->child(i));

        if (driver->doesInputMatchAnalysis(analysisAction->text(),
                                           childItemForSingleInput, false))
          {
          analysisAction->setEnabled(true);
          }
        }
      }
    }
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

// --------------------------------------------------------------------------
void voMainWindow::loadAnalysisScripts()
{
  voApplication * app = voApplication::application();
  QString scriptPath = app->homeDirectory() + "/" + Visomics_INSTALL_SCRIPTS_DIR;
  QDir scriptDir(scriptPath);

  QStringList filters;
  filters << "*.xml";
  QFileInfoList xmlFiles = scriptDir.entryInfoList(filters);

  voAnalysisDriver *driver = voApplication::application()->analysisDriver();
  foreach(QFileInfo xmlFileInfo, xmlFiles)
    {
    // make sure the corresponding R script exists
    QString xmlFileName = xmlFileInfo.absoluteFilePath();
    QString rScriptFileName = xmlFileName;
    rScriptFileName.replace(".xml", ".R");
    if (!scriptDir.exists(rScriptFileName))
      {
      qWarning() << xmlFileName << "exists but" << rScriptFileName << "does not";
      continue;
      }
    driver->loadAnalysisFromScript(xmlFileName, rScriptFileName);
    }
}

// --------------------------------------------------------------------------
void voMainWindow::onCustomAnalysisAdded(const QString &analysisName)
{
  Q_D(voMainWindow);
  QAction * action = new QAction(analysisName, this);
  action->setEnabled(false);
  d->AnalysisActionMapper.setMapping(action, analysisName);
  connect(action, SIGNAL(triggered()), &d->AnalysisActionMapper, SLOT(map()));
  d->menuAnalysis->addAction(action);
}
