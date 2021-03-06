#ifndef BPWINDOW_H
#define BPWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include "bitprophet.h"
#include "coinbaseaccountsetupwindow.h"
#include "gdaxaccountsetupwindow.h"

namespace Ui {
    class bpWindow;
}

class bitProphet;
class coinbaseAccountSetupWindow;
class gdaxAccountSetupWindow;
class QComboBox;

class bpWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit bpWindow(QWidget *parent = 0);
    ~bpWindow();
    bitProphet *mProphet;
    ///////////////////////////////////
    // TextEdits
    QTextEdit *getStatusOutput();
    QTextEdit *getCoinbaseTabLog();
    QTextEdit *getAutoTraderBTCLog();
    QTextEdit *getAutoTraderLTCLog();
    QTextEdit *getAutoTraderETHLog();
    QTextEdit *getGdaxAutoTraderBTCLog();
    QTextEdit *getGdaxAutoTraderLTCLog();
    QTextEdit *getGdaxAutoTraderETHLog();
    QTextEdit *getWebSocketLiveOutput();
    /////////////////////////////////////
    coinbaseAccountSetupWindow *getAccSetupWindow();
    QComboBox* getAccountsCombo();
    QComboBox* getDepositFromPayMethodCombo();
    QComboBox* getDepositToPayMethodCombo();
    QComboBox* getWithdrawFromPayMethodCombo();
    QComboBox* getWithdrawToPayMethodCombo();
    QComboBox* getBuySpotWithPaymentMethodComboBox();
    QComboBox* getSellSpotWithPaymentMethodComboBox();
    QComboBox* getBuySpotTypeComboBox();
    QComboBox* getSellSpotTypeComboBox();
    QPushButton* getDepositFromPayMethodButton();
    QPushButton* getWithdrawToPayMethodButton();
    QPushButton* getQuoteBuySpotForPaymentMethodButton();
    QPushButton* getQuoteSellSpotForPaymentMethodButton();
    QPushButton* getBuySpotForPaymentMethodButton();
    QPushButton* getSellSpotForPaymentMethodButton();
    QPushButton* getRefreshTransactionsButton();
    QPushButton *getRefreshAutoSpotTraderTransactionsButton();
    QPushButton *getXferFromCbWalletButton();
    QPushButton *getXferToCbWalletButton();
    ///////////////////////////////////////
    QLineEdit *getDepositFromPayMethodAmount();
    QLineEdit *getWithdrawToPayMethodAmount();
    QLineEdit *getXferFromCoinbaseAmount();
    QLineEdit *getXferToCoinbaseAmount();
    QLineEdit* getAutoGdaxMinCryptoBuy();
    QLineEdit* getAutoGdaxMinUSDBuy();
    /////////////////////////////////////
    void killAccSetupWindow();
    void killGdaxAccSetupWindow();
    QWidget *getAccountsTab();
    QWidget *getGdaxAccountsTab();
    QWidget *getTransactionTab();
    QTableWidget *getAccountTablePlacer();
    QTableWidget *getGdaxAccountTablePlacer();
    QTableWidget *getTransactionTablePlacer();
    QTableWidget *getCbAutoSpotTraderTransactionTablePlacer();
    QLabel *getAccountsNativeBalanceTotalLabel();
    // Spot Price Labels
//    QLabel *getBtcSpotPriceLabel();
//    QLabel *getLtcSpotPriceLabel();
//    QLabel *getEthSpotPriceLabel();
    QLabel *getBuySpotFeeLabel();
    QLabel *getBuySpotTotalLabel();
    QLabel *getSellSpotFeeLabel();
    QLabel *getSellSpotTotalLabel();
//    QLabel *getBtcSpotBuyPriceLabel();
//    QLabel *getLtcSpotBuyPriceLabel();
//    QLabel *getEthSpotBuyPriceLabel();
//    QLabel *getBtcSpotSellPriceLabel();
//    QLabel *getLtcSpotSellPriceLabel();
//    QLabel *getEthSpotSellPriceLabel();
    QLabel *getGdaxBtcPriceLabel();
    QLabel *getGdaxBtcAskLabel();
    QLabel *getGdaxBtcBidLabel();
    QLabel *getGdaxLtcPriceLabel();
    QLabel *getGdaxLtcAskLabel();
    QLabel *getGdaxLtcBidLabel();
    QLabel *getGdaxEthPriceLabel();
    QLabel *getGdaxEthAskLabel();
    QLabel *getGdaxEthBidLabel();
    ///////////////////////////////////
    //Request Stats Labels
    QLabel *getCbStatRequestsLabel();
    QLabel *getCbStatResponsesLabel();
    QLabel *getCbStatProcessedLabel();
    QLabel *getCbStatDestroyedLabel();
    QLabel *getGdaxStatRequestsLabel();
    QLabel *getGdaxStatResponsesLabel();
    QLabel *getGdaxStatProcessedLabel();
    QLabel *getGdaxStatDestroyedLabel();
    QLabel *getWebSocketStateLabel();
    QLabel *getBtcUsdPriceLabel();
    ////////////////////////////////////
    QLineEdit *getBuySpotAmount();
    QLineEdit *getSellSpotAmount();
    QLineEdit *getPlaceLimitBuyAmount();
    QLineEdit *getPlaceLimitBuyPrice();
    QLineEdit *getPlaceLimitSellAmount();
    QLineEdit *getPlaceLimitSellPrice();
    QLineEdit *getAutoGdaxMinPercentProfit();
    QLineEdit *getAutoGdaxMaxUSDBuy();
    QLineEdit *getAutoGdaxMinBuyInterval();
    QFrame *getCbBTCPricePlacer();
    QFrame *getCbLTCPricePlacer();
    QFrame *getCbETHPricePlacer();
    QWidget *getChartsTab();
    QWidget *getBtcChartTab();
    QWidget *getLtcChartTab();
    QWidget *getEthChartTab();
    QWidget *getBchChartTab();
    ////////////////////////
    // Checkboxes
    QCheckBox *getAutoSpotTraderEnabledCheckBox();
    QCheckBox *getAutoRefreshGdaxBalanceEnabledCheckBox();
    QCheckBox *getAutoRefreshCbBalanceEnabledCheckBox();
    QCheckBox *getGdaxAutoTradeEnabledCheckBox();
    QCheckBox *getWebSocketSubBtcUsd();
    QCheckBox *getWebSocketSubLtcUsd();
    QCheckBox *getWebSocketSubEthUsd();
    QCheckBox *getWebSocketSubBchUsd();
    /////////////////////////////////////////////
    // ComboBoxes
    QComboBox *getXferFromCbWalletComboBox();
    QComboBox *getXferToCbWalletComboBox();
    QComboBox *getXferFromCbWalletTargetComboBox();
    QComboBox *getXferToCbWalletSourceComboBox();
    QComboBox *getPlaceGdaxLimitSellTargetComboBox();
    QComboBox *getPlaceGdaxLimitBuySourceComboBox();
    QComboBox *getPlaceGdaxLimitBuyTypeComboBox();
    QComboBox *getPlaceGdaxLimitSellTypeComboBox();
    QComboBox *getGdaxAccountComboBox();
    ////////////////////////////////////
private:
    Ui::bpWindow *ui;

    coinbaseAccountSetupWindow *mCbSetupWin;
    gdaxAccountSetupWindow *mGdaxSetupWin;
private slots:
    void accountSetupClicked();
    void accountSetupGDAXClicked();
//    void on_mAutoSpotTradeEnabledCheckBox_toggled(bool checked);
    void on_mAutoRefreshBalanceCheckBox_toggled(bool checked);
    void on_mGdaxTransferFrom_released();
    void on_mGdaxTransferTo_released();
    void on_mPlaceGdaxLimitBuy_released();
    void on_mPlaceGdaxLimitSell_released();
    void on_mCancelAllGdaxOrders_released();
    void on_mAutoGDAXTradeEnabledCheckBox_toggled(bool checked);
    void on_pushButton_released();
};

#endif // BPWINDOW_H
