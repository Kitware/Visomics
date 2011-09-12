/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QEventLoop>
#include <QMetaEnum>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScriptEngine>
#include <QScriptValue>

// Visomics includes
#include "voKEGGUtils.h"


//----------------------------------------------------------------------------
bool voKEGGUtils::queryServer(const QUrl& requestURL, QByteArray* responseData)
{
  static QNetworkAccessManager networkManager;

  //Send request
  QNetworkRequest request(requestURL);
  QNetworkReply* reply = networkManager.get(request);

  //Wait for reply
  QEventLoop loop;
  QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec();

  // Check for errors
  if(reply->error() != QNetworkReply::NoError)
    {
    const QMetaObject &mo = QNetworkReply::staticMetaObject;
    QString errorString = mo.enumerator(mo.indexOfEnumerator("NetworkError")).valueToKey(reply->error());
    qWarning() << "Error: Could not connect to server:" << errorString;
    return false;
    }
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if(statusCode != 200)
    {
    qWarning() << "Error: Server returned non-success status code:" << statusCode;
    return false;
    }

  //Return
  *responseData = reply->readAll();
  return true;
}

//----------------------------------------------------------------------------
bool voKEGGUtils::dataToJSON(const QByteArray& rawData, QScriptValue* jsonData)
{
  static QScriptEngine scriptEngine;

  // Security issue: we are running potentially arbitrary script
  // code retreived from an unsecure network connection.
  // Does QScriptEngine sufficiently sandbox its evaluation environment?
  // Also, we do not ensure that the QScriptValue will eventually halt.

  QScriptValue scriptValue = scriptEngine.evaluate(QString(rawData));
  if(scriptEngine.hasUncaughtException())
    {
    qWarning() << "Error: improperly formatted JSON";
    return true;
    }

  *jsonData = scriptValue;
  return true;
}
