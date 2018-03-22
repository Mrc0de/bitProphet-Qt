#ifndef GDAXWEBSOCKETHANDLER_H
#define GDAXWEBSOCKETHANDLER_H

#include <QObject>
#include <QString>
#include <QTextEdit>
#include <QUrl>
#include <QtWebSockets/QtWebSockets>
#include "gdaxapihandler.h"

class gdaxApiHandler;

class gdaxWebSocketHandler : public QObject {
    Q_OBJECT
public:
    explicit gdaxWebSocketHandler(gdaxApiHandler *parent);
    ~gdaxWebSocketHandler();
    void say(QString sayThis);    
    QString trimDollarStringCents(QString amount); //open for public use (stupid, but useful)
private:
    QUrl mWSHost;
    QWebSocket *mWS;
    gdaxApiHandler *mParent;
    void parseTicker(QJsonObject ticker);
private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void updateSubscriptionsFromUI();
//    void onSslErrors(const QList<QSslError> &errors);
//    void on_mSubPricesButton_released();
    void onTextMessageReceived(QString message);
};

#endif // GDAXWEBSOCKETHANDLER_H
