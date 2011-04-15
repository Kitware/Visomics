
// Qt includes
#include <QApplication>
#include <QTimer>
#include <QDebug>

// Visomics includes
#include <voDelimitedTextImportDialog.h>

// STD includes
#include <cstdlib>

//-----------------------------------------------------------------------------
int voDelimitedTextImportDialogTest(int argc, char * argv [])
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(VisomicsApp);

  // Read file
  QString filename("/home/jchris/Projects/Bioinformatics/Data/UNC/All_conc_kitware_transposed.csv");

  voDelimitedTextImportDialog dialog;
  dialog.setFileName(filename);
  
  dialog.show();

  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &dialog, SLOT(accept()));
    autoExit.start(1000);
    }

  int status = dialog.exec();
  if (status != QDialog::Accepted)
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

