#include "gdaxwebsockethandler.h"

gdaxWebSocketHandler::gdaxWebSocketHandler(gdaxApiHandler *parent) : QObject(parent), mWSHost(QStringLiteral("wss://ws-feed.gdax.com")), mWS(NULL) {
    //just in case
    mParent = parent;
    say("WebSocket Host: " + mWSHost.toString());
    mWS = new QWebSocket();
    QObject::connect(mWS, &QWebSocket::connected, this, &gdaxWebSocketHandler::onConnected);

    mWS->open(QUrl(mWSHost));
    if ( mWS->errorString() != "Unknown error") {
        say("ERROR: " + mWS->errorString());
    }
    mParent->mParent->mParent->getWebSocketLiveOutput()->document()->setMaximumBlockCount(256);
}

gdaxWebSocketHandler::~gdaxWebSocketHandler() {
    if ( mWS != NULL ) { QObject::disconnect(mWS,&QWebSocket::disconnected, this, &gdaxWebSocketHandler::onDisconnected); mWS->close(); mWS->deleteLater();}
    say("Websocket Shutting Down...");
}

///////////
// METHODS
void gdaxWebSocketHandler::say(QString sayThis) {
     mParent->mParent->mParent->getWebSocketLiveOutput()->append(sayThis);
     mParent->mParent->mParent->getWebSocketLiveOutput()->verticalScrollBar()->setValue(mParent->mParent->mParent->getWebSocketLiveOutput()->verticalScrollBar()->maximum());
}

QString gdaxWebSocketHandler::trimDollarStringCents(QString amount) {
    QString p;
    if ( amount.indexOf(".",0) != -1 ) {
        p = amount.mid(amount.indexOf(".",0)+1,2) ;
        if ( p.length() == 1 ) { p+="0"; }
        p = amount.mid(0,amount.indexOf(".",0)) + "." + p;
    } else {
        p = amount + ".00";
    }
    return p;
}

void gdaxWebSocketHandler::parseTicker(QJsonObject ticker) {
    QString tProdId;
    QString tTime;
    QString tLastSize;
    QString tBid;
    QString tPrice;
    QString tAsk;
    tProdId = ticker["product_id"].toString();
    tTime = ticker["time"].toString();
    tLastSize = ticker["last_size"].toString();
    tBid = trimDollarStringCents(ticker["best_bid"].toString());
    tPrice = trimDollarStringCents(ticker["price"].toString());
    tAsk = trimDollarStringCents(ticker["best_ask"].toString());
    mParent->mParent->getDb()->addToGdaxPriceHistory(tProdId.mid(0,tProdId.indexOf('-')),tPrice,tAsk,tBid);
    if ( tProdId == "BTC-USD" ) {
        mParent->mParent->mParent->getGdaxBtcAskLabel()->setText(tAsk);
        mParent->mParent->mParent->getGdaxBtcBidLabel()->setText(tBid);
        mParent->mParent->mParent->getGdaxBtcPriceLabel()->setText(tPrice);
    } else if ( tProdId == "LTC-USD" ) {
        mParent->mParent->mParent->getGdaxLtcAskLabel()->setText(tAsk);
        mParent->mParent->mParent->getGdaxLtcBidLabel()->setText(tBid);
        mParent->mParent->mParent->getGdaxLtcPriceLabel()->setText(tPrice);
    } else if ( tProdId == "ETH-USD" ) {
        mParent->mParent->mParent->getGdaxEthAskLabel()->setText(tAsk);
        mParent->mParent->mParent->getGdaxEthBidLabel()->setText(tBid);
        mParent->mParent->mParent->getGdaxEthPriceLabel()->setText(tPrice);
    } else if ( tProdId == "BCH-USD" ) {
        //need some BCH labels I guess....
        //        mParent->mParent->mParent->getGdaxBchAskLabel()->setText(tAsk);
        //        mParent->mParent->mParent->getGdaxBchBidLabel()->setText(tBid);
        //        mParent->mParent->mParent->getGdaxBchPriceLabel()->setText(tPrice);
    }
    if ( tLastSize.length() > 0 ) {
//        say(tProdId + " - $" + tPrice + " ~ $"+ tAsk +" | Vs | $" + tBid + " ~ [Size: "+ tLastSize +"] ");
    }
}

/////////
// SLOTS
void gdaxWebSocketHandler::onConnected(){
    say("WebSocket CONNECTED!");
    mParent->mParent->mParent->getWebSocketStateLabel()->setText("CONNECTED");
    mParent->mParent->mParent->getWebSocketStateLabel()->setStyleSheet("QLabel { background-color : green; color : black; }");
    connect(mWS, &QWebSocket::textMessageReceived, this, &gdaxWebSocketHandler::onTextMessageReceived);
    connect(mWS, &QWebSocket::disconnected, this, &gdaxWebSocketHandler::onDisconnected);
    updateSubscriptionsFromUI();
}

void gdaxWebSocketHandler::onDisconnected() {
    say("WebSocket: DISCONNECTED!");
    mParent->mParent->mParent->getWebSocketStateLabel()->setText("DEAD");
    mParent->mParent->mParent->getWebSocketStateLabel()->setStyleSheet("QLabel { background-color : red; color : white; }");
    //Reconnect
    say("Reconnecting: " + mWSHost.toString());
    if ( mWS != NULL ) { QObject::disconnect(mWS,&QWebSocket::textMessageReceived, this, &gdaxWebSocketHandler::onTextMessageReceived);QObject::disconnect(mWS,&QWebSocket::connected, this, &gdaxWebSocketHandler::onConnected); QObject::disconnect(mWS,&QWebSocket::disconnected, this, &gdaxWebSocketHandler::onDisconnected); mWS->deleteLater(); }
    mWS = new QWebSocket();
    QObject::connect(mWS, &QWebSocket::connected, this, &gdaxWebSocketHandler::onConnected);
    mWS->open(QUrl(mWSHost));
    if ( mWS->errorString() != "Unknown error") {
        say("ERROR: " + mWS->errorString());
    }
}

void gdaxWebSocketHandler::onTextMessageReceived(QString message) {
    //say("<"+mWSHost.toString()+"> " + message);
    //I supposed we better dismantle this shitstorm
    QJsonParseError error;
    QByteArray unparsed(message.toUtf8());
    QJsonDocument jsonData = QJsonDocument::fromJson(unparsed,&error);
    QJsonArray jsonArr = jsonData.array();
    QJsonObject jsonObj = jsonData.object();
    if ( jsonObj["type"].toString() == "ticker") {
        parseTicker(jsonObj);
    }
}

void gdaxWebSocketHandler::updateSubscriptionsFromUI(){
    QList<QString> subs;
    if ( mParent->mParent->mParent->getWebSocketSubBtcUsd()->isChecked() ) { subs.append("BTC-USD"); }
    if ( mParent->mParent->mParent->getWebSocketSubLtcUsd()->isChecked() ) { subs.append("LTC-USD"); }
    if ( mParent->mParent->mParent->getWebSocketSubEthUsd()->isChecked() ) { subs.append("ETH-USD"); }
    if ( mParent->mParent->mParent->getWebSocketSubBchUsd()->isChecked() ) { subs.append("BCH-USD"); }
    QString subMsg("{\"type\": \"subscribe\",\"product_ids\": [ ");
    for (int z=0;z<subs.count();z++){
        subMsg.append("\"");
        subMsg.append(subs.at(z));
        subMsg.append("\"");
        if ( z+1 != subs.count() ) { subMsg.append(","); }
    }
    subMsg.append("],\"channels\": [{\"name\": \"ticker\"}]}");
    say(subMsg);
    mWS->sendTextMessage(subMsg);
}










