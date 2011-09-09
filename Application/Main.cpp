
// Qt includes
#include <QFile>
#include <QMessageBox>
#include <QTimer>

// CTK includes
#include <ctkCallback.h>

// Visomics includes
#include "voApplication.h"
#include "voConfigure.h" // For Visomics_VERSION
#include "voMainWindow.h"

namespace
{
//----------------------------------------------------------------------------
void checkRHome(void * data)
{
  voApplication * app = voApplication::application();
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
    QMessageBox::critical(reinterpret_cast<voMainWindow*>(data),
                          QString("Visomics %1").arg(Visomics_VERSION), message);
    app->quit();
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

  ctkCallback checkRHomeCallback;
  checkRHomeCallback.setCallback(checkRHome);
  checkRHomeCallback.setCallbackData(&mainwindow);
  QTimer::singleShot(0, &checkRHomeCallback, SLOT(invoke()));
  
  return app.exec();
}
