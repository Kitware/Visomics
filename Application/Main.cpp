/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

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
void popupMessage(QWidget * parent, const QString& message)
{
  QMessageBox::critical(parent, QString("Visomics %1").arg(Visomics_VERSION), message);
  qCritical() << message;
}
//----------------------------------------------------------------------------
void popupMessageAndQuit(QWidget * parent, const QString& message)
{
  QString tmp = message;
  popupMessage(parent, tmp.append("<br><br><big><img src=\":/Icons/Bulb.png\">&nbsp;The application will be terminated</big>"));
  voApplication::application()->quit();
}

//----------------------------------------------------------------------------
void checkRPrerequisites(void * data)
{
  voApplication * app = voApplication::application();
  voMainWindow * mainWindow = reinterpret_cast<voMainWindow*>(data);
  QString message;
  if (!QFile::exists(app->rHome()))
    {
    message = "<big>GnuR expects <b>R_HOME</b> environement variable.</big><br><br>";
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
    popupMessageAndQuit(mainWindow, message);
    return;
    }

//  QString rscriptFilePath = QLatin1String(Visomics_SOURCE_DIR) + "/Utilities/GetRExternalPackages.R";
//  if(app->isInstalled())
//    {
//    rscriptFilePath = app->homeDirectory() + "/" + Visomics_INSTALL_RSCRIPTS_DIR + "/GetRExternalPackages.R";
//    }
//  qDebug() << "Evaluating R script:" << rscriptFilePath;
//  QFile rscriptFile(rscriptFilePath);
//  if (!rscriptFile.exists())
//    {
//    popupMessage(mainWindow, QString("<big>Script doesn't exist</big><br><br>%1").arg(rscriptFilePath));
//    return;
//    }
//  if (!rscriptFile.open(QFile::ReadOnly))
//    {
//    popupMessage(mainWindow, QString("<big>Failed to read script</big><br><br>%1").arg(rscriptFilePath));
//    return;
//    }
//  QTextStream in(&rscriptFile);
//  QString rscript = in.readAll();
//  vtkNew<vtkRInterface> rInterface;
//  char outputBuffer[2048];
//  rInterface->OutputBuffer(outputBuffer, 2048);
//  rInterface->EvalRscript(rscript.toLatin1(), /* showRoutput= */ false);
//  qDebug() << outputBuffer;

//  message = "<big>Problem running R script</big><br><br>";
//  message.append(rscriptFilePath).append("<br>");
//  message.append("<ul>");

//  bool installationFailed = false;
//  QString package = "pls";
//  QString requiredBy = "<b>PLSStatistics</b> analysis";
//  if (!QString(outputBuffer).contains(QString("Package '%1' found").arg(package)))
//    {
//    message.append(QString("<li>R package <b>%1</b> required by %2 is not installed</li><br>")
//                   .arg(package).arg(requiredBy));
//    installationFailed = true;
//    }
//  package = "preprocessCore";
//  requiredBy = "<b>Quantile</b> normalization";
//  if (!QString(outputBuffer).contains(QString("Package '%1' found").arg(package)))
//    {
//    message.append(QString("<li>R package <b>%1</b> required by %2 is not installed</li><br>")
//                   .arg(package).arg(requiredBy));
//    installationFailed = true;
//    }
//  message.append("</ul>");
//  if(installationFailed)
//    {
//    message.append("<br><big><img src=\":/Icons/Bulb.png\">&nbsp;The application will start but not all"
//                   " functionalities will be available</big>");
//    popupMessage(mainWindow, message);
//    }
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
