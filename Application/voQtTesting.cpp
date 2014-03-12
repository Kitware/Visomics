#include "voQtTesting.h"

#include "pqTestUtility.h"
#include "pqEventObserver.h"
#include "pqEventSource.h"

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextStream>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QtDebug>

class XMLEventObserver : public pqEventObserver
{
  QXmlStreamWriter* XMLStream;
  QString XMLString;

public:
  XMLEventObserver(QObject* p) : pqEventObserver(p)
  {
  this->XMLStream = NULL;
  }

  ~XMLEventObserver()
    {
    delete this->XMLStream;
    }

protected:
  virtual void setStream(QTextStream* stream)
    {
    if (this->XMLStream)
      {
      this->XMLStream->writeEndElement();
      this->XMLStream->writeEndDocument();
      delete this->XMLStream;
      this->XMLStream = NULL;
      }
    if (this->Stream)
      {
      *this->Stream << this->XMLString;
      }
    this->XMLString = QString();
    pqEventObserver::setStream(stream);
    if (this->Stream)
      {
      this->XMLStream = new QXmlStreamWriter(&this->XMLString);
      this->XMLStream->setAutoFormatting(true);
      this->XMLStream->writeStartDocument();
      this->XMLStream->writeStartElement("events");
      }
    }

  virtual void onRecordEvent(const QString& widget, const QString& command,
    const QString& arguments)
    {
    if(this->XMLStream)
      {
      this->XMLStream->writeStartElement("event");
      this->XMLStream->writeAttribute("widget", widget);
      this->XMLStream->writeAttribute("command", command);
      this->XMLStream->writeAttribute("arguments", arguments);
      this->XMLStream->writeEndElement();
      }
    }
};

class XMLEventSource : public pqEventSource
{
  typedef pqEventSource Superclass;
  QXmlStreamReader *XMLStream;

public:
  XMLEventSource(QObject* p): Superclass(p) { this->XMLStream = NULL;}
  ~XMLEventSource() { delete this->XMLStream; }

protected:
  virtual void setContent(const QString& xmlfilename)
    {
    delete this->XMLStream;
    this->XMLStream = NULL;

    QFile xml(xmlfilename);
    if (!xml.open(QIODevice::ReadOnly))
      {
      qDebug() << "Failed to load " << xmlfilename;
      return;
      }
    QByteArray data = xml.readAll();
    this->XMLStream = new QXmlStreamReader(data);
    /* This checked for valid event objects, but also caused the first event
     * to get dropped. Commenting this out in the example. If you wish to report
     * empty XML test files a flag indicating whether valid events were found is
     * probably the best way to go.
    while (!this->XMLStream->atEnd())
      {
      QXmlStreamReader::TokenType token = this->XMLStream->readNext();
      if (token == QXmlStreamReader::StartElement)
        {
        if (this->XMLStream->name() == "event")
          {
          break;
          }
        }
      } */
    if (this->XMLStream->atEnd())
      {
      qDebug() << "Invalid xml" << endl;
      }
    return;
    }

  int getNextEvent(QString& widget, QString& command, QString&
    arguments)
    {
    if (this->XMLStream->atEnd())
      {
      return DONE;
      }
    while (!this->XMLStream->atEnd())
      {
      QXmlStreamReader::TokenType token = this->XMLStream->readNext();
      if (token == QXmlStreamReader::StartElement)
        {
        if (this->XMLStream->name() == "event")
          {
          break;
          }
        }
      }
    if (this->XMLStream->atEnd())
      {
      return DONE;
      }
    widget = this->XMLStream->attributes().value("widget").toString();
    command = this->XMLStream->attributes().value("command").toString();
    arguments = this->XMLStream->attributes().value("arguments").toString();
    return SUCCESS;
    }
};

//-----------------------------------------------------------------------------
voQtTesting::voQtTesting(QWidget* newParent):Superclass(newParent)
{
  this->TestUtility = new pqTestUtility(this);
  this->TestUtility->addEventObserver("xml", new XMLEventObserver(this));
  this->TestUtility->addEventSource("xml", new XMLEventSource(this));
}

//-----------------------------------------------------------------------------
voQtTesting::~voQtTesting()
{
  delete this->TestUtility;
}

//-----------------------------------------------------------------------------
void voQtTesting::startRecording()
{
  QString filename = QFileDialog::getSaveFileName (this, "Test File Name",
    QString(), "XML Files (*.xml)");
  if (!filename.isEmpty())
    {
    this->TestUtility->recordTests(filename);
    }
}

//-----------------------------------------------------------------------------
void voQtTesting::stopRecording()
{
  this->TestUtility->stopRecords(1);
}

//-----------------------------------------------------------------------------
void voQtTesting::play()
{
  QString filename = QFileDialog::getOpenFileName (this, "Test File Name",
    QString(), "XML Files (*.xml)");
  this->playTest(filename);
}

//-----------------------------------------------------------------------------
void voQtTesting::playTest(QString filename)
{
  if (!filename.isEmpty())
    {
    this->TestUtility->playTests(filename);
    }
}
