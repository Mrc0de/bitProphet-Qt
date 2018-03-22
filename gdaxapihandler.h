#ifndef GDAXAPIHANDLER_H
#define GDAXAPIHANDLER_H

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCipher>
#include <QLoggingCategory>
#include <QSslSocket>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>


#include "bpwindow.h"
#include "bitprophet.h"
#include "gdaxaccount.h"
#include "gdaxapirequest.h"
#include "gdaxapiresponse.h"
#include "gdaxwallettable.h"
#include "gdaxwebsockethandler.h"

class bpWindow;
class bitProphet;
class gdaxAccount;
class gdaxApiRequest;
class gdaxApiResponse;
class gdaxWalletTable;
class gdaxWebSocketHandler;

class gdaxApiHandler : public QObject {
    Q_OBJECT
public:
    explicit gdaxApiHandler(bitProphet *parent);
    ~gdaxApiHandler();    
    void say(QString sayThis);    
    QString getGdaxApiKey();
    QString getGdaxApiSecret();
    QString getGdaxPassPhrase();
    void xferFromCoinbaseToGdax(QString fromAccId, QString amount,QString currency);
    void xferFromGdaxToCoinbase(QString fromAccId, QString amount,QString currency);
    void placeGdaxLimitBuy(QString prodId,QString size, QString price);
    void placeGdaxLimitSell(QString prodId,QString size, QString price);
    void placeGdaxAutoTraderLimitBuy(QString prodId,QString size, QString price,int autoTradeId);
    void placeGdaxAutoTraderLimitSell(QString prodId,QString size, QString price, int autoTradeId);
    void cancelAllGdaxOrders();
    void fetchGdaxFillsForOrderId(QString orderId);
    void fetchGdaxSellFillsForOrderId(QString orderId);
    void processResponse( gdaxApiResponse *resp );    
    void process404(gdaxApiResponse *resp);
    bitProphet *mParent;
    gdaxAccount *mAccount;
private:    
    QString mPtrName;    
    QList<QString> mProductIds;    
    void listGdaxAccounts();
    void listAccountProcessResponse(gdaxApiResponse *resp );
    void listCoinbaseAccountsAvailableToGdax();
    void listCoinbaseAccountsProcessResponse(gdaxApiResponse *resp );    
    void fetchGdaxPriceProcessResponse(gdaxApiResponse *resp,QString productId);
    void fetchGdaxFillsForOrderIdProcessResponse(gdaxApiResponse *resp);
    void fetchGdaxSellFillsForOrderIdProcessResponse(gdaxApiResponse *resp);
    void placeGdaxAutoTraderLimitBuyProcessResponse(gdaxApiResponse *resp);
    void placeGdaxAutoTraderLimitSellProcessResponse(gdaxApiResponse *resp);
    void xferFromGdaxToCoinbaseProcessResponse(gdaxApiResponse *resp);
    gdaxWalletTable *mWalletTableWidget;
    void fetchGdaxPrice(QString prodId);
    gdaxWebSocketHandler *mWSHandler;
signals:

public slots:    
    void listGdaxAccountsSlot();
private slots:

    void fetchGdaxPriceSlotBtc();
    void fetchGdaxPriceSlotLtc();
    void fetchGdaxPriceSlotEth();
};

#endif // GDAXAPIHANDLER_H
