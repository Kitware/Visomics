
// Qt includes
#include <QApplication>
#include <QTimer>
#include <QDebug>

// Visomics includes
#include <voDelimitedTextPreview.h>
#include <voDelimitedTextImportWidget.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int voDelimitedTextImportWidgetTest(int argc, char * argv [])
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

  voDelimitedTextPreviewModel model;
  model.setFileName(filename);

  voDelimitedTextPreview * documentPreviewWidget = new voDelimitedTextPreview;
  documentPreviewWidget->setModel(&model);

  voDelimitedTextImportWidget w;
  w.setDelimitedTextPreviewModel(&model);
  w.insertWidget(documentPreviewWidget, voDelimitedTextImportWidget::RowsAndColumnsGroupBox);
  w.show();

  QTimer autoExit;
  //if (argc < 2 || QString(argv[1]) != "-I")
  //  {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
  //  }
  return app.exec();
}

