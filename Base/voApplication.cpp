
//Qt includes
#include <QDebug>
#include <QMainWindow>

// Visomics includes
#include "voAnalysisDriver.h"
#include "voApplication.h"
#include "voDataModel.h"
#include "voIOManager.h"
#include "voViewManager.h"
#include "voAnalysisFactory.h"
#include "voViewFactory.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkTable.h>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

// --------------------------------------------------------------------------
class voApplicationPrivate
{
public:
  voApplicationPrivate();
  
  bool                   Initialized;
  bool                   ExitWhenDone;
  voDataModel            DataModel;
  voAnalysisDriver       AnalysisDriver;
  voIOManager            IOManager;
  voViewManager          ViewManager;
  voAnalysisFactory      AnalysisFactory;
  voViewFactory          ViewFactory;
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
// voApplication methods

// --------------------------------------------------------------------------
voApplication::voApplication(int & argc, char ** argv):
    Superclass(argc, argv), d_ptr(new voApplicationPrivate)
{
  Q_D(voApplication);
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
  
  // TODO Parse command line arguments
  //d->parseArguments();

  d->Initialized = true;
  exitWhenDone = d->ExitWhenDone;
}

// --------------------------------------------------------------------------
voDataModel* voApplication::dataModel()const
{
  Q_D(const voApplication);
  return const_cast<voDataModel*>(&d->DataModel);
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
