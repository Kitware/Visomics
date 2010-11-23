
// Qt includes
#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>

// Visomics includes
#include "voMainWindow.h"
#include "ui_voMainWindow.h"
#include "voAnalysisPanel.h"
#include "voCSVReader.h"
#include "voCorrelationGraphView.h"
#include "voPCA.h"
#include "voPCAStatistics.h"
#include "voPCAProjectionPlot.h"

// --------------------------------------------------------------------------
class voMainWindowPrivate: public Ui_voMainWindow
{
  Q_DECLARE_PUBLIC(voMainWindow);
protected:
  voMainWindow* const q_ptr;
public:
  voMainWindowPrivate(voMainWindow& object);

  QWidget *          AnalysisOptionWidget;
  QWidget *          AnalysisWidget;
  voAnalysisPanel *  AnalysisPanel;

  QMap<QString, voAnalysis*>  InputDataMap;
  QMap<QString, voAnalysis*>  PCAResultMap;

  QString   CurrentFileName;
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

  d->Ui_voMainWindow::setupUi(this);

  d->AnalysisPanel = new voAnalysisPanel( );
  this->setCentralWidget(d->AnalysisPanel);

  //Connect DataBrowser widget
  connect(d->DataBrowserWidget, SIGNAL(itemSelectionChanged()),
          this, SLOT(selectItemTreeWidget()));

  // setup actions
  d->actionFileOpen->setShortcut(QKeySequence::Open);
  connect(d->actionFileOpen, SIGNAL(triggered()), this, SLOT(open()));
  connect(d->actionFileExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(d->actionHelpAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(d->actionAnalysisPCA, SIGNAL(triggered()), this, SLOT(pca()));
  connect(d->actionAnalysisRawData, SIGNAL(triggered()), this, SLOT(loadRawData()));

  // Initialize status bar
  this->statusBar()->showMessage(tr(""), 2000);

  this->mode = RAW;
}

// --------------------------------------------------------------------------
voMainWindow::~voMainWindow()
{
}

// --------------------------------------------------------------------------
void voMainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open table data"), "*.csv",
                                   tr("CSV files (*.csv)"));
  if (!fileName.isEmpty())
    {
      if (loadFile(fileName) )
        {
        addData( fileName );
        }
    }
}

// --------------------------------------------------------------------------
bool voMainWindow::loadFile( const QString &fileName )
{
  Q_D(voMainWindow);

  std::cout << "Load....\t" << fileName.toStdString() << std::endl;
  voCSVReader * csvReader  = new voCSVReader();
  csvReader->setFileName( fileName );
  csvReader->update();

  QString shownName = this->strippedName(fileName);

  //Add it to the map
  std::cout << "Add the file to the inputDataMap..." << shownName.toStdString() << std::endl;
  d->InputDataMap[shownName] = csvReader;

  d->AnalysisPanel->SetAnalysis ( csvReader );

  statusBar()->showMessage(tr("File loaded"), 2000);

  return true;
}

// --------------------------------------------------------------------------
void voMainWindow::setCurrentFile(const QString &fileName)
{
    QString shownName = tr("Untitled");
    if (!fileName.isEmpty()) {
        shownName = this->strippedName(fileName);
    }

    this->setWindowTitle(tr("%1[*] - %2").arg(shownName)
                         .arg(tr("Spreadsheet")));
}

// --------------------------------------------------------------------------
QString voMainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

// --------------------------------------------------------------------------
void voMainWindow::addData( const QString  &fileName )
{
  Q_D(voMainWindow);

  if (fileName.isEmpty())
    {
    return;
    }

  QString shownName = this->strippedName(fileName);
  QTreeWidgetItem *root;
  if (d->DataBrowserWidget->topLevelItemCount() == 0)
    {
    root = new QTreeWidgetItem(d->DataBrowserWidget);
    root->setText(0, tr("Loaded Data"));
    d->DataBrowserWidget->setItemExpanded(root, true);
    }
  else
    {
    root = d->DataBrowserWidget->topLevelItem(0);
    }

  QTreeWidgetItem *newItem = new QTreeWidgetItem(root);
  newItem->setText(0, shownName);
  d->DataBrowserWidget->setCurrentItem(newItem);

  d->CurrentFileName = shownName;
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
void voMainWindow::pca()
{
  Q_D(voMainWindow);

  this->mode = PCA;

  if ( d->CurrentFileName != "" )
    {
    voPCAStatistics* pca = new voPCAStatistics();
    voView* view = d->AnalysisPanel->currentView();
    view->update();
    pca->setInput(view);
    pca->update();

    //Add it to the map
    d->PCAResultMap[d->CurrentFileName] = pca;
    d->AnalysisPanel->SetAnalysis ( pca );
    }
}

// --------------------------------------------------------------------------
void voMainWindow::loadRawData()
{
  Q_D(voMainWindow);

  std::cout << "LoadRaw data action" << std::endl;
  this->mode = RAW;
  if ( d->CurrentFileName != "" )
    {
    std::cout << "Switch to raw data " << d->CurrentFileName.toStdString() << std::endl;
    d->AnalysisPanel->SetAnalysis ( d->InputDataMap[ d->CurrentFileName ] );
    voView* view = d->AnalysisPanel->currentView();
    view->update();
    }
}

// --------------------------------------------------------------------------
void voMainWindow::selectItemTreeWidget()
{
  Q_D(voMainWindow);

  std::cout << "Selected item in the tree widget" << std::endl;

  QList<QTreeWidgetItem *> selectedItems = d->DataBrowserWidget->selectedItems();

  if ( selectedItems.isEmpty() )
    {
    return;
    } 

  int row = d->DataBrowserWidget->indexOfTopLevelItem( selectedItems.first() );
  if ( row == 0 )
    {
    return;
    }

  QMutableListIterator<QTreeWidgetItem *> i(selectedItems);
  while (i.hasNext()) 
    {
    QString name = i.next()->text(0);
    d->CurrentFileName = name;
    std::cout << " Selected element: " << name.toStdString() << std::endl;
    if( this->mode  == RAW  )
      {
      std::cout << "Mode is RAW" << std::endl;
      if ( d->InputDataMap.find( name ) == d->InputDataMap.end() )
        {
        this->loadFile( name );
        }
      else
        {
        d->AnalysisPanel->SetAnalysis( d->InputDataMap[name] );
        voView* view = d->AnalysisPanel->currentView();
        view->update();
        }
      }
    if( this->mode == PCA )
      {
      std::cout << "Mode is PCA" << std::endl;
      if ( d->PCAResultMap.find( name ) == d->PCAResultMap.end() )
        {
        std::cout << "Data not yet processed\n" << std::endl;
        if ( d->InputDataMap.find( name ) == d->InputDataMap.end() )
          {
          //Data not yet loaded
          this->loadFile( name );
          }
   
        //Change to table view and then call pca
        d->AnalysisPanel->SetAnalysis( d->InputDataMap[name] );
        this->pca();
        }
      else
        {
        d->AnalysisPanel->SetAnalysis( d->PCAResultMap[name] );
        }
      }
    }
}
