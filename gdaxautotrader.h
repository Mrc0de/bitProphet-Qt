#ifndef GDAXAUTOTRADER_H
#define GDAXAUTOTRADER_H

#include <QObject>
#include "bitprophet.h"
#include "timetool.h"

class bitProphet;
class timeTool;

class gdaxAutoTrader : public QObject {
    Q_OBJECT
public:
    explicit gdaxAutoTrader(bitProphet *parent);
    ~gdaxAutoTrader();
    void say(QString sayThis);
    void sayGdaxAutoTrader(QString sayThis,QString whichBox="ALL");
    QString findHighestValue(QList<QString> hayStack);
    QString findLowestValue(QList<QString> hayStack);
    QTextEdit *mBTCLog;
    QTextEdit *mLTCLog;
    QTextEdit *mETHLog;
    QList<QString> mTradeTypes; //actively sell these coins
    QList<QString> mBuyTypes; //actively buy these coins
    QString mUSDStartAmount;
    double mMinUSDBuyAmount;
    double mMaxUSDBuyAmount;
    double mMinPercentProfit; //DECIMAL VERSION -> 1% = 0.01
    QString mLastBuyPriceBTC;
    QString mLastBuyPriceLTC;
    QString mLastBuyPriceETH;
    QString mLastBuyTimeBTC;
    QString mLastBuyTimeLTC;
    QString mLastBuyTimeETH;
    QString mLastSellPriceBTC;
    QString mLastSellPriceLTC;
    QString mLastSellPriceETH;
    QString mLastSellTimeBTC;
    QString mLastSellTimeLTC;
    QString mLastSellTimeETH;
    double mMinCryptoBuyAmount;
    double mLastSellExpireAfter;
    double mLastBuyExpireAfter;
private:
    bitProphet *mParent;    
public slots:
    void autoTradeCheck();
private slots:    
    void checkForBuyFills();
    void checkForSellFills();
};

#endif // GDAXAUTOTRADER_H
