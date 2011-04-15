
// Qt includes
#include <QApplication>
#include <QTimer>
#include <QDebug>

// Visomics includes
#include <voDelimitedTextImportWidget.h>

// STD includes
#include <cstdlib>

//-----------------------------------------------------------------------------
int voDelimitedTextImportWidgetTest(int argc, char * argv [])
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(VisomicsApp);

  // Read file
  QString filename("/home/jchris/Projects/Bioinformatics/Data/UNC/All_conc_kitware_transposed.csv");

  voDelimitedTextImportWidget w;
  w.show();

  w.setFileName(filename);

  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
    }
  return app.exec();
}

