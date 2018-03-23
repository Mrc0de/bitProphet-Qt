#ifndef BPDATABASE_H
#define BPDATABASE_H

#include <QObject>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "spotprice.h"
#include "bpsplinechart.h"

#include "bitprophet.h"
#include "coinbaseaccount.h"
#include "gdaxaccount.h"

class bitProphet;
class coinbaseAccount;
class spotPrice;
class bpSplineChart;
class gdaxAccount;

class bpDatabase : public QObject {
    Q_OBJECT
public:
    explicit bpDatabase(QObject *parent);
    ~bpDatabase();
    QString mPtrName;
    bool fileExists();
    bool hasAccountsTable();
    bool hasTable(QString tableName);
    void createDatabase();
    bool createAccountsTable();
    bool createGdaxAccountsTable();
    bool createGdaxPriceHistoryTable();
    bool createCbSpotPriceHistoryTable();
    bool createAutoSpotTradeHistoryTable();
    bool createGdaxTraderHistoryTable();
    void addToCbSpotPriceHistory( QString coin, QString price );
    void addToGdaxPriceHistory( QString coin, QString price,QString ask, QString bid );
    void insertAccount( QString name, QString apiKey, QString apiSecret, bool defaultAccount=0,  QString exchange="coinbase");
    void insertGdaxAccount( QString name, QString apiKey, QString apiSecret, bool defaultAccount, QString passPhrase, QString exchange="GDAX");
    void insertAutoSpotTrade( QString coin, QString type, QString boughtAt,QString soldAt,QString amount,QString status);
    void insertGdaxAutoTrade( QString coin, QString type, QString status, QString amount, QString buyPrice, QString buyTotal, QString sellTarget, QString sellTotal, QString minProfitPct, QString minProfitUsd);
    void updateAutoSpotTradeSoldAt( QString id, QString soldAt);
    void updateRowById( QString id, QString table, QString column, QString newValue );
    int getLastIdForTable(QString table);
    QList<QString> getAccountList();
    QList<QString> getGdaxAccountList();
    void getGdaxAutoBuysPlaced(QString coin,QList<QString> *buys);
    void getGdaxAutoSellsPosted(QString coin,QList<QString> *sells);
    QString getGdaxAutoTradeIdByOrderId(QString orderId);
    QString getGdaxAutoTradeHistoryValueById(QString id,QString column);
    void deleteAccount(QString id );
    void deleteGdaxAccount(QString id );
    QString getDefaultAccountId();
    QString getDefaultGdaxAccountId();
    void loadAccountByName(coinbaseAccount *target, QString accountName);
    void loadAccountById(coinbaseAccount *target, QString id);
    void loadGdaxAccountById(gdaxAccount *target, QString id);
    void getBtcSpotPriceHistoryLast(int howManyMax, bpSplineChart *chart);
    void getLtcSpotPriceHistoryLast(int howManyMax,bpSplineChart *chart);
    void getEthSpotPriceHistoryLast(int howManyMax,bpSplineChart *chart);
    void getSpotPriceHistoryLast(QString coin,int maxResults, QList<QString> *priceList);
    void getGdaxPriceHistoryLast(QString coin,int maxResults, QList<QString> *priceList,QList<QString> *askList,QList<QString> *bidList);
    void getGdaxPriceHistoryFrom(QString coin,QString dateTimeFrom, int maxResults, QList<QString> *priceList,QList<QString> *askList,QList<QString> *bidList,QList<QString> *tsList);
    void getAutoSpotBuysForSaleList(QList<QString> *idList,QList<QString> *boughtAtList,QList<QString> *coinAmountList,QString coin);
private:
    void say(QString sayThis);

signals:

public slots:

};

#endif // BPDATABASE_H
