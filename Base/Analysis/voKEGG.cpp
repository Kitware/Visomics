
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

  QString keggURL("http://" + this->stringParameter("host") + "/kegg-pathway?term=");
  QNetworkAccessManager manager;
  QScriptEngine engine;

  vtkNew<vtkTable> output;
  output->AddColumn(table->GetColumn(0));

  vtkNew<vtkStringArray> pathwaysColumn;
  pathwaysColumn->SetName("KEGG Pathways");
  for (vtkIdType i = 0; i < table->GetNumberOfRows(); ++i)
    {
    QEventLoop loop;
    QNetworkRequest request;
    QString name = QString::fromStdString(table->GetValue(i, 0).ToString());
    qDebug() << "i:" << i <<  ", name:" << name;
    request.setUrl(QUrl(keggURL + QUrl::toPercentEncoding(name)));
    request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
    QNetworkReply *reply = manager.get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QString result = QString(reply->readAll());
    QScriptValue sc = engine.evaluate(result);
    QStringList paths;
    if (sc.isArray())
      {
      for (int i = 0; i < sc.property("length").toInteger(); ++i)
        {
        QScriptValue v = sc.property(i);
        paths << v.property("id").toString();
        qDebug() << "  id:" << v.property("id").toString()
                 << ", name:" << v.property("name").toString();
        }
      }
    pathwaysColumn->InsertNextValue(paths.join(";").toStdString());
    }
  output->AddColumn(pathwaysColumn.GetPointer());

  // output->Dump();

  this->setOutput("pathways", new voTableDataObject("pathways", output.GetPointer()));
  return true;
}
