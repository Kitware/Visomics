
// Qt includes
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QTimer>

// CTK includes
#include <ctkCallback.h>

// Visomics includes
#include "voApplication.h"
#include "voConfigure.h" // For Visomics_VERSION, Visomics_INSTALL_RSCRIPTS_DIR
#include "voMainWindow.h"

// VTK includes
#include <vtkRInterface.h>
#include <vtkNew.h>

namespace
{
//----------------------------------------------------------------------------
void popupMessageAndQuit(QWidget * parent, const QString& message)
{
  QMessageBox::critical(parent, QString("Visomics %1").arg(Visomics_VERSION), message);
  qCritical() << message;
  voApplication::application()->quit();
}

//----------------------------------------------------------------------------
void checkRPrerequisites(void * data)
{
  voApplication * app = voApplication::application();
  voMainWindow * mainWindow = reinterpret_cast<voMainWindow*>(data);
  if (!QFile::exists(app->rHome()))
    {
    QString message("GnuR requires <b>R_HOME</b> environement variable to be set properly.<br><br>");
    if (app->rHome().isEmpty())
      {
      message.append("R_HOME is either not set or set to an empty value.<br>");
      }
    else
      {
      message.append("R_HOME is set to a nonexistent directory:<br>");
      message.append("<br>");
      message.append(app->rHome()).append("<br>");
      }
    message.append("<br>The application will be terminated.");
    popupMessageAndQuit(mainWindow, message);
    return;
    }

  QString rscriptFilePath = QLatin1String(Visomics_SOURCE_DIR) + "/Utilities/GetRExternalPackages.R";
  if(app->isInstalled())
    {
    rscriptFilePath = app->homeDirectory() + "/" + Visomics_INSTALL_RSCRIPTS_DIR + "/GetRExternalPackages.R";
    }
  qDebug() << "Evaluating R script:" << rscriptFilePath;
  QFile rscriptFile(rscriptFilePath);
  if (!rscriptFile.exists())
    {
    popupMessageAndQuit(mainWindow, QString("Script %1 doesn't exist.").arg(rscriptFilePath));
    return;
    }
  if (!rscriptFile.open(QFile::ReadOnly))
    {
    popupMessageAndQuit(mainWindow, QString("Failed to read script %1").arg(rscriptFilePath));
    return;
    }
  QTextStream in(&rscriptFile);
  QString rscript = in.readAll();
  vtkNew<vtkRInterface> rInterface;
  char outputBuffer[2048];
  rInterface->OutputBuffer(outputBuffer, 2048);
  rInterface->EvalRscript(rscript.toLatin1(), /* showRoutput= */ false);
  qDebug() << outputBuffer;

  QString message("Problem running R script: ");
  message.append(rscriptFilePath).append("<br><br>");

  foreach(const QString& package, QStringList() << "pls" << "preprocessCore")
    {
    if (!QString(outputBuffer).contains(QString("Package '%1' found").arg(package)))
      {
      message.append(QString("Failed to install R package: %1").arg(package));
      popupMessageAndQuit(mainWindow, message);
      return;
      }
    }
}

} // end of anonymous namespace

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  voApplication app(argc, argv);

  Q_INIT_RESOURCE(VisomicsApp);
  Q_INIT_RESOURCE(VisomicsBase);

  bool exitWhenDone = false;
  app.initialize(exitWhenDone);
  if (exitWhenDone)
    {
    return EXIT_SUCCESS;
    }
  
  voMainWindow mainwindow;
  mainwindow.show();

  ctkCallback callback;
  callback.setCallback(checkRPrerequisites);
  callback.setCallbackData(&mainwindow);
  QTimer::singleShot(0, &callback, SLOT(invoke()));
  
  return app.exec();
}
