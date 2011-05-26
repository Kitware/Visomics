
// Qt includes
//#include <QApplication>
//#include <QCursor>
#include <QNetworkReply>

// QtSOAP includes
#include <QtSoapHttpTransport>
#include <QtSoapType>

// Visomics includes
#include "voSoapClient.h"

//----------------------------------------------------------------------------
class voSoapClientPrivate
{
public:

  QEventLoop BlockingLoop;
  QtSoapHttpTransport Http;

  QString Xmlns;
  QString Path;
};

//----------------------------------------------------------------------------
voSoapClient::voSoapClient(int port, const QString& host, const QString& path, const QString& xmlns)
  : d_ptr(new voSoapClientPrivate())
{
  Q_D(voSoapClient);

  d->Xmlns = xmlns;
  d->Path = path;

  connect(&d->Http, SIGNAL(responseReady()), this, SLOT(responseReady()));

  d->Http.setHost(host, /* useSecureHTTP= */false, port);
}

//----------------------------------------------------------------------------
voSoapClient::~voSoapClient()
{

}

//----------------------------------------------------------------------------
void voSoapClient::responseReady()
{
  Q_D(voSoapClient);
  d->BlockingLoop.exit();
}

//----------------------------------------------------------------------------
const QtSoapMessage & voSoapClient::submitSoapRequest(const QString& methodName,
                                                      QtSoapType* soapType)
{
  QList<QtSoapType*> list;
  if (soapType)
    {
    list << soapType;
    }
  return this->submitSoapRequest(methodName, list);
}

//----------------------------------------------------------------------------
const QtSoapMessage & voSoapClient::submitSoapRequest(const QString& methodName,
                                                      const QList<QtSoapType*>& soapTypes)
{
  Q_D(voSoapClient);

  // qDebug() << "invoking method:" << methodName;

  QtSoapMessage request;
  request.setMethod(QtSoapQName(methodName, d->Xmlns));
  if(!soapTypes.isEmpty())
    {
    foreach(QtSoapType * soapType, soapTypes)
      {
      request.addMethodArgument(soapType);
//      qDebug() << "\tArgument " << soapType->typeName() + ":" + soapType->name().name() << "added";
      }
    }
//  qDebug() << request.toXmlString();

  d->Http.submitRequest(request, d->Path);

//  qDebug() << "Submitted request:" << methodName;

//  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  d->BlockingLoop.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);

//  QApplication::restoreOverrideCursor();

  //qDebug() << "Reply error: " << reply->errorString();
  //qDebug() << reply->readAll();
  const QtSoapMessage& response = d->Http.getResponse();
//  qDebug() << "\t=> OK";
  return response;
}
