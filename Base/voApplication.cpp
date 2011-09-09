
// Qt includes
#include <QDebug>
#include <QDir>
#include <QMainWindow>
#include <QWebSettings>

// CTK includes
#include <ctkErrorLogModel.h>
#include <ctkErrorLogQtMessageHandler.h>
#include <ctkErrorLogStreamMessageHandler.h>
#include <ctkVTKErrorLogMessageHandler.h>

// Visomics includes
#include "voAnalysisDriver.h"
#include "voApplication.h"
#include "voConfigure.h" // For Visomics_INSTALL_BIN_DIR, Visomics_INSTALL_LIB_DIR
#include "voDataModel.h"
#include "voIOManager.h"
#include "voNormalization.h"
#include "voRegistry.h"
#include "voViewManager.h"
#include "voAnalysisFactory.h"
#include "voViewFactory.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkTable.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

// --------------------------------------------------------------------------
class voApplicationPrivate
{
public:
  voApplicationPrivate();

  void init();

  QString discoverHomeDirectory();

  QString                HomeDirectory;
  bool                   Initialized;
  bool                   ExitWhenDone;
  voDataModel            DataModel;
  voAnalysisDriver       AnalysisDriver;
  voIOManager            IOManager;
  voViewManager          ViewManager;
  voAnalysisFactory      AnalysisFactory;
  voRegistry             NormalizerRegistry;
  voViewFactory          ViewFactory;

  QSharedPointer<ctkErrorLogModel> ErrorLogModel;
};

// --------------------------------------------------------------------------
// voApplicationPrivate methods

// --------------------------------------------------------------------------
voApplicationPrivate::voApplicationPrivate()
{
  this->Initialized = false;
  this->ExitWhenDone = false;
}

// --------------------------------------------------------------------------
void voApplicationPrivate::init()
{
  // Instantiate ErrorLogModel
  this->ErrorLogModel = QSharedPointer<ctkErrorLogModel>(new ctkErrorLogModel);
  this->ErrorLogModel->setLogEntryGrouping(true);
  this->ErrorLogModel->setTerminalOutputEnabled(true);

  this->ErrorLogModel->registerMsgHandler(new ctkErrorLogQtMessageHandler);
  this->ErrorLogModel->registerMsgHandler(new ctkErrorLogStreamMessageHandler);
  this->ErrorLogModel->registerMsgHandler(new ctkVTKErrorLogMessageHandler);

  this->ErrorLogModel->setAllMsgHandlerEnabled(true);

  this->HomeDirectory = this->discoverHomeDirectory();
}

//-----------------------------------------------------------------------------
QString voApplicationPrivate::discoverHomeDirectory()
{
  QDir binDir(QApplication::instance()->applicationDirPath());
  if (binDir.dirName() != QLatin1String(Visomics_INSTALL_BIN_DIR)
      && binDir.dirName() != QLatin1String(Visomics_INSTALL_LIB_DIR))
    {
    binDir.cdUp();
    }
  binDir.cdUp();
  return binDir.canonicalPath();
}

// --------------------------------------------------------------------------
// voApplication methods

// --------------------------------------------------------------------------
voApplication::voApplication(int & argc, char ** argv):
    Superclass(argc, argv), d_ptr(new voApplicationPrivate)
{
  Q_D(voApplication);
  d->init();
  connect(&d->DataModel, SIGNAL(viewSelected(QString)),
          &d->ViewManager, SLOT(createView(const QString&)));

  connect(&d->AnalysisDriver,
          SIGNAL(analysisAddedToObjectModel(voAnalysis*)),
          &d->DataModel,
          SLOT(setActiveAnalysis(voAnalysis*)));
}

// --------------------------------------------------------------------------
voApplication::~voApplication()
{
}

//-----------------------------------------------------------------------------
voApplication* voApplication::application()
{
  voApplication* app = qobject_cast<voApplication*>(QApplication::instance());
  return app;
}

// --------------------------------------------------------------------------
void voApplication::initialize(bool& exitWhenDone)
{
  Q_D(voApplication);

  // Register normalization methods
  this->normalizerRegistry()->registerMethod("Log2", Normalization::applyLog2);
  this->normalizerRegistry()->registerMethod("Quantile", Normalization::applyQuantile);
  
  QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

  // TODO Parse command line arguments
  //d->parseArguments();

  d->Initialized = true;
  exitWhenDone = d->ExitWhenDone;
}

// --------------------------------------------------------------------------
bool voApplication::initialized()const
{
  Q_D(const voApplication);
  return d->Initialized;
}

//-----------------------------------------------------------------------------
bool voApplication::isInstalled()const
{
  Q_D(const voApplication);
  return !QFile::exists(d->HomeDirectory + "/CMakeCache.txt");
}

// --------------------------------------------------------------------------
QString voApplication::homeDirectory()const
{
  Q_D(const voApplication);
  return d->HomeDirectory;
}

// --------------------------------------------------------------------------
QString voApplication::rHome()const
{
  return vtksys::SystemTools::GetEnv("R_HOME");
}

// --------------------------------------------------------------------------
voDataModel* voApplication::dataModel()const
{
  Q_D(const voApplication);
  return const_cast<voDataModel*>(&d->DataModel);
}

//-----------------------------------------------------------------------------
ctkErrorLogModel* voApplication::errorLogModel()const
{
  Q_D(const voApplication);
  return d->ErrorLogModel.data();
}

// --------------------------------------------------------------------------
voAnalysisDriver* voApplication::analysisDriver()const
{
  Q_D(const voApplication);
  return const_cast<voAnalysisDriver*>(&d->AnalysisDriver);
}

// --------------------------------------------------------------------------
voIOManager* voApplication::ioManager()const
{
  Q_D(const voApplication);
  return const_cast<voIOManager*>(&d->IOManager);
}

// --------------------------------------------------------------------------
voViewManager* voApplication::viewManager()const
{
  Q_D(const voApplication);
  return const_cast<voViewManager*>(&d->ViewManager);
}

// --------------------------------------------------------------------------
voAnalysisFactory* voApplication::analysisFactory()const
{
  Q_D(const voApplication);
  return const_cast<voAnalysisFactory*>(&d->AnalysisFactory);
}

// --------------------------------------------------------------------------
voRegistry* voApplication::normalizerRegistry()const
{
  Q_D(const voApplication);
  return const_cast<voRegistry*>(&d->NormalizerRegistry);
}

// --------------------------------------------------------------------------
voViewFactory* voApplication::viewFactory()const
{
  Q_D(const voApplication);
  return const_cast<voViewFactory*>(&d->ViewFactory);
}

// --------------------------------------------------------------------------
QMainWindow* voApplication::mainWindow()const
{
  QMainWindow* mainwindow = 0;
  foreach(QWidget * w, this->topLevelWidgets())
    {
    mainwindow = qobject_cast<QMainWindow*>(w);
    if (mainwindow)
      {
      break;
      }
    }
  return mainwindow;
}
