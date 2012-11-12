#include "voJavascriptBridge.h"
#include <QDesktopServices>
#include <QUrl>

// --------------------------------------------------------------------------


voJavascriptBridge::voJavascriptBridge()
{
}

void voJavascriptBridge::openURL(const QString & urlStr)
{
 QUrl url(urlStr);
 QDesktopServices::openUrl(url);
}

