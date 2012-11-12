#ifndef __voJavascriptBridge_h
#define __voJavascriptBridge_h

#include <QObject>
#include <QString>


class voJavascriptBridge: public QObject
{
Q_OBJECT
public:
   voJavascriptBridge();
   Q_INVOKABLE void openURL(const QString  & urlStr);
};

#endif
