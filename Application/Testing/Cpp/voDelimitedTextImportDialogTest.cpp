
// Qt includes
#include <QApplication>
#include <QTimer>
#include <QDebug>

// Visomics includes
#include <voDelimitedTextImportDialog.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int voDelimitedTextImportDialogTest(int argc, char * argv [])
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(VisomicsApp);
  
  if (argc < 2)
    {
    // TODO Add a better to handle arguments. May be by using ctkCommandLineParser
    std::cerr << "Missing an argument !\n"
              << "Usage: \n"
              << "  " << argv[0] << " </path/to/data.csv>" << std::endl;
    return EXIT_FAILURE;
    }

  // Read file
  QString filename(argv[1]);

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

