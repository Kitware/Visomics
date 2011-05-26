

#ifndef __voSoapClient_h
#define __voSoapClient_h

// Qt includes
#include <QObject>

class QtSoapMessage;
class QtSoapType;
class voSoapClientPrivate;

class voSoapClient : public QObject
{
  Q_OBJECT
public:
  voSoapClient(int port, const QString& host, const QString& path, const QString& xmlns);
  virtual ~voSoapClient();

  const QtSoapMessage & submitSoapRequest(const QString& methodName, const QList<QtSoapType*>& soapTypes);
  const QtSoapMessage & submitSoapRequest(const QString& methodName, QtSoapType* soapType = 0);

private slots:

  void responseReady();

private:

  const QScopedPointer<voSoapClientPrivate> d_ptr;

  Q_DECLARE_PRIVATE(voSoapClient);
  Q_DISABLE_COPY(voSoapClient);
};


#endif

