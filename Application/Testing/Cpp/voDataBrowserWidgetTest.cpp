
// Qt includes
#include <QApplication>
#include <QTimer>
#include <QDebug>

// Visomics includes
#include <voDataBrowserWidget.h>
#include <voDataModel.h>
#include <voDataModelItem.h>
#include <voCSVReader.h>
#include <voInputFileDataObject.h>

// STD includes
#include <cstdlib>

//-----------------------------------------------------------------------------
int voDataBrowserWidgetTest(int argc, char * argv [])
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

  voDataBrowserWidget w;
  voDataModel model;
  w.setModel(&model);
  w.setSelectionModel(model.selectionModel());

  w.show();

  // Read file
  QString filename(argv[1]);
  voCSVReader reader;
  reader.setFileName(filename);
  reader.update();

  // Add corresponding dataObject to model
  voInputFileDataObject * dataObject = new voInputFileDataObject();
  dataObject->setName("test");
  dataObject->setFileName(filename);
  dataObject->setData(reader.output());
  model.addDataObject(dataObject);

  QStandardItem * container1 = model.addContainer("PCA");
  Q_ASSERT(container1);
  QStandardItem * container2 = model.addContainer("PCA2");
  Q_ASSERT(container2);

  model.addContainer("Outputs", container1);

  model.addContainer("Views", container1);

  model.addContainer("Outputs", container2);

  model.addContainer("Views", container2);

  QTimer autoExit;
  //if (argc < 2 || QString(argv[1]) != "-I")
  //  {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
  //  }
  return app.exec();
}

