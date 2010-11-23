
// Qt includes
#include <QApplication>

// Visomics includes
#include "voMainWindow.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  
  voMainWindow mainwindow;
  mainwindow.show();
  
  return app.exec();
}
