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

} // end of anonymous namespace

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  voApplication app(argc, argv);

  Q_INIT_RESOURCE(VisomicsApp);
#ifdef USE_ARBOR_BRAND
  Q_INIT_RESOURCE(ArborBase);
#else
  Q_INIT_RESOURCE(VisomicsBase);
#endif

  bool exitWhenDone = false;
  app.initialize(exitWhenDone);
  if (exitWhenDone)
    {
    return EXIT_SUCCESS;
    }

  voMainWindow mainwindow;
  mainwindow.show();

  ctkCallback callback;
  callback.setCallbackData(&mainwindow);
  QTimer::singleShot(0, &callback, SLOT(invoke()));

  if (argc > 1)
    {
    mainwindow.playTest(QString(argv[1]));
    return 0;
    }

  return app.exec();
}
