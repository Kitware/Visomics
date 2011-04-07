
// Qt includes
#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScriptEngine>
#include <QScriptValue>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voKEGG.h"
#include "voTableDataObject.h"

// VTK includes
#include <vtkArrayToTable.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voKEGG methods

// --------------------------------------------------------------------------
voKEGG::voKEGG():
    Superclass()
{
//  Q_D(voKEGG);
}

// --------------------------------------------------------------------------
voKEGG::~voKEGG()
{
}

// --------------------------------------------------------------------------
void voKEGG::setInputInformation()
{
  this->addInputType("input", "vtkTable");
}

// --------------------------------------------------------------------------
void voKEGG::setOutputInformation()
{
  this->addOutputType("pathways", "vtkTable",
                      "voTableView", "Table (pathways)");
}

// --------------------------------------------------------------------------
void voKEGG::setParameterInformation()
{
  QList<QtProperty*> kegg_parameters;

  // KEGG host
  kegg_parameters << this->addStringParameter("host", tr("Host name"), "paraviewweb.kitware.com:88");

  this->addParameterGroup("KEGG parameters", kegg_parameters);
}

// --------------------------------------------------------------------------
bool voKEGG::execute()
{
//  Q_D(voKEGG);

  vtkTable* table =  vtkTable::SafeDownCast(this->input()->data());
  if (!table)
    {
    qWarning() << "Input is Null";
    return false;
    }

  QNetworkAccessManager manager;
  QScriptEngine engine;
  vtkNew<vtkTable> output;
  output->AddColumn(table->GetColumn(0));
//  vtkNew<vtkStringArray> header;
//  header->SetName("header");
  //header->InsertNextValue("KEGG Pathways");
  vtkSmartPointer<vtkStringArray> path = vtkSmartPointer<vtkStringArray>::New();
  path->SetName("KEGG Pathways");
  for (vtkIdType i = 1; i < table->GetNumberOfRows(); ++i)
    {
    QEventLoop loop;
    QNetworkRequest request;
    QString name = static_cast<const char*>(table->GetValue(i,0).ToString());
    request.setUrl(QUrl("http://" + this->stringParameter("host") + "/kegg-pathway?term=" + QUrl::toPercentEncoding(name)));
    request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
    QNetworkReply *reply = manager.get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QString result = QString(reply->readAll());
    QScriptValue sc = engine.evaluate(result);
    QString paths;
    if (sc.isArray())
      {
      for (int i = 0; i < sc.property("length").toInteger(); ++i)
        {
        QScriptValue v = sc.property(i);
        paths += v.property("id").toString();
        paths += ";";
        cerr << v.property("id").toString().toStdString() << endl;
        cerr << v.property("name").toString().toStdString() << endl;
        }
      }
//    vtkNew<vtkStringArray> path;
//    path->SetName(name.toStdString().c_str());
    path->InsertNextValue(paths.toStdString());
    }
    output->AddColumn(path);

  this->setOutput("pathways", new voTableDataObject("pathways", output.GetPointer()));
  return true;
}
