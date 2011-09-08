#ifndef __voKEGGUtils_h
#define __voKEGGUtils_h

class QByteArray;
class QScriptEngine;
class QUrl;

namespace voKEGGUtils
{

  bool queryServer(const QUrl& requestURL, QByteArray* responseData);

  bool dataToJSON(const QByteArray& rawData, QScriptValue* jsonData);
}

#endif
