#ifndef CBAPIHANDLER_H
#define CBAPIHANDLER_H

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
#include "coinbaseaccount.h"
#include "cbapirequest.h"
#include "cbapiresponse.h"
#include "cbwallettable.h"
#include "cbtransactiontable.h"

class bpWindow;
class bitProphet;
class coinbaseAccount;
class cbApiResponse;
class cbWalletTable;
class cbTransactionTable;



class cbApiHandler : public QObject {
    Q_OBJECT
public:
    explicit cbApiHandler(QObject *parent = nullptr);
    ~cbApiHandler();
    void listWalletAccounts();    
    bitProphet *mParentProphet;
    QString getCoinbaseApiKey();
    QString getCoinbaseApiSecret();
    void say( QString sayThis );
    void processResponse( cbApiResponse *resp );
    void processBadListAccountsResponse();
    void processBuySpotQuoteResponse( cbApiResponse *resp );
    void processSellSpotQuoteResponse( cbApiResponse *resp );
    void processBuyAutoSpotResponse(cbApiResponse *resp);
    void listAccountProcessResponse( cbApiResponse *resp );
    void listPayMethodProcessResponse( cbApiResponse *resp );
    void listAccounts();    
    void listPaymentMethods();
    QString trimPriceStringDecimal( QString priceString);
    void cbTabLog(QString logString);
    void buyAutoSpot(QString total,QString coin);
    coinbaseAccount *mAccount; //api handler 'has' only one account(at a time) even if more exist (TODO: make switchable)
private:
    QString mPtrName;
    QString mCbApiKey;
    QString mCbApiSecret;    
    cbWalletTable *mWalletTableWidget;
    cbTransactionTable *mTransactionTable;
public slots:
    void fetchSpotPrices();
    void fetchSpotPrices2();
    void fetchSpotPrices3();
    void fetchBTCSpotPrice();
    void fetchLTCSpotPrice();
    void fetchETHSpotPrice();
    void fetchBTCSpotBuyPrice();
    void fetchLTCSpotBuyPrice();
    void fetchETHSpotBuyPrice();
    void fetchBTCSpotSellPrice();
    void fetchLTCSpotSellPrice();
    void fetchETHSpotSellPrice();
    void depositFromButtonSlot();
    void withdrawToButtonSlot();
//    void QuoteBuySpotClicked();
//    void QuoteSellSpotClicked();
    void disableBuySpotButton();
    void disableSellSpotButton();
//    void buySpotClicked();
//    void sellSpotClicked();
    void fetchTransactions();
    void processBuySpotResponse( cbApiResponse *resp );
    void processSellSpotResponse( cbApiResponse *resp );    
    void processFetchTransactionsResponse(cbApiResponse *resp);
signals:



};

#endif // CBAPIHANDLER_H
