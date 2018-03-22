#include "bpwindow.h"
#include "ui_bpwindow.h"

bpWindow::bpWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::bpWindow), mCbSetupWin(NULL),mGdaxSetupWin(NULL) {
    ui->setupUi(this);
    //bpWindow creates the actual bitProphet
    //bitProphet controls everything
    //bpWindow handles display tasks for bitProphet
    mProphet = new bitProphet(this);

    //Bind Menus
    connect(ui->actionAccount,&QAction::triggered,this, &bpWindow::accountSetupClicked);
    connect(ui->actionGDAX_Account,&QAction::triggered,this, &bpWindow::accountSetupGDAXClicked);
}

///////////
//decon
///////////
bpWindow::~bpWindow()
{
    //These will always exist.
    delete mProphet; //delete before ui    
    delete ui;
}

///////////
//Methods
///////////
QTextEdit * bpWindow::getStatusOutput() {
    return ui->mStatusOutput;
}

QTextEdit * bpWindow::getWebSocketLiveOutput() {
    return ui->mWebSocketLiveOutput;
}


QTextEdit* bpWindow::getCoinbaseTabLog() { return ui->mCoinbaseTabLog; }

//QTextEdit* bpWindow::getAutoTraderBTCLog() { return ui->mAutoSpotBTCLog; }
//QTextEdit* bpWindow::getAutoTraderLTCLog() { return ui->mAutoSpotLTCLog; }
//QTextEdit* bpWindow::getAutoTraderETHLog() { return ui->mAutoSpotETHLog; }
QTextEdit* bpWindow::getGdaxAutoTraderBTCLog() { return ui->mGdaxAutoBTCLog; }
QTextEdit* bpWindow::getGdaxAutoTraderLTCLog() { return ui->mGdaxAutoLTCLog; }
QTextEdit* bpWindow::getGdaxAutoTraderETHLog() { return ui->mGdaxAutoETHLog; }


QTableWidget *bpWindow::getAccountTablePlacer() {
    return ui->cbAccountTablePlacer;
}

QTableWidget *bpWindow::getCbAutoSpotTraderTransactionTablePlacer() {
    return ui->mCbAutoSpotTraderTransactionTablePlacer;
}

QTableWidget *bpWindow::getTransactionTablePlacer() {
    return ui->mCbTransactionTablePlacer;
}

QTableWidget *bpWindow::getGdaxAccountTablePlacer() {
    return ui->mGdaxAccountTablePlacer;
}

QWidget *bpWindow::getAccountsTab() {
    return ui->mCoinbaseTab;
}

QWidget *bpWindow::getGdaxAccountsTab() {
    return ui->mGdaxTab;
}

QLabel *bpWindow::getAccountsNativeBalanceTotalLabel() {
    return ui->mTotalNativeValue;
}

QLabel *bpWindow::getWebSocketStateLabel() {
    return ui->mWebSocketStateLabel;
}

//QLabel *bpWindow::getBtcSpotPriceLabel() {
//    return ui->mBtcSpotPriceLabel;
//}

//QLabel *bpWindow::getLtcSpotPriceLabel() {
//    return ui->mLtcSpotPriceLabel;
//}

//QLabel *bpWindow::getEthSpotPriceLabel() {
//    return ui->mEthSpotPriceLabel;
//}

//QLabel *bpWindow::getBtcSpotBuyPriceLabel() {
//    return ui->mBtcSpotBuyPriceLabel;
//}

//QLabel *bpWindow::getLtcSpotBuyPriceLabel() {
//    return ui->mLtcSpotBuyPriceLabel;
//}

//QLabel *bpWindow::getEthSpotBuyPriceLabel() {
//    return ui->mEthSpotBuyPriceLabel;
//}

//QLabel *bpWindow::getBtcSpotSellPriceLabel() {
//    return ui->mBtcSpotSellPriceLabel;
//}

//QLabel *bpWindow::getLtcSpotSellPriceLabel() {
//    return ui->mLtcSpotSellPriceLabel;
//}

//QLabel *bpWindow::getEthSpotSellPriceLabel() {
//    return ui->mEthSpotSellPriceLabel;
//}



QLabel *bpWindow::getBuySpotFeeLabel() {
    return ui->mBuySpotFeeLabel;
}

QLabel *bpWindow::getBuySpotTotalLabel() {
    return ui->mBuySpotTotalLabel;
}

QLabel *bpWindow::getSellSpotFeeLabel() {
    return ui->mSellSpotFeeLabel;
}

QLabel *bpWindow::getSellSpotTotalLabel() {
    return ui->mSellSpotTotalLabel;
}

QLabel *bpWindow::getCbStatRequestsLabel() {
    return ui->mCbStatRequests;
}

QLabel *bpWindow::getCbStatResponsesLabel() {
    return ui->mCbStatResponses;
}

QLabel *bpWindow::getCbStatProcessedLabel() {
    return ui->mCbStatProcessed;
}

QLabel *bpWindow::getCbStatDestroyedLabel() {
    return ui->mCbStatDestroyed;
}

QLabel *bpWindow::getGdaxStatRequestsLabel() {
    return ui->mGdaxStatRequests;
}

QLabel *bpWindow::getGdaxStatResponsesLabel() {
    return ui->mGdaxStatResponses;
}

QLabel *bpWindow::getGdaxStatProcessedLabel() {
    return ui->mGdaxStatProcessed;
}

QLabel *bpWindow::getGdaxStatDestroyedLabel() {
    return ui->mGdaxStatDestroyed;
}

QLabel *bpWindow::getGdaxBtcPriceLabel() {
    return ui->mGdaxBtcPriceLabel;
}

QLabel *bpWindow::getGdaxBtcAskLabel() {
    return ui->mGdaxBtcAskLabel;
}

QLabel *bpWindow::getGdaxBtcBidLabel() {
    return ui->mGdaxBtcBidLabel;
}

QLabel *bpWindow::getGdaxLtcPriceLabel() {
    return ui->mGdaxLtcPriceLabel;
}

QLabel *bpWindow::getGdaxLtcAskLabel() {
    return ui->mGdaxLtcAskLabel;
}

QLabel *bpWindow::getGdaxLtcBidLabel() {
    return ui->mGdaxLtcBidLabel;
}

QLabel *bpWindow::getGdaxEthPriceLabel() {
    return ui->mGdaxEthPriceLabel;
}

QLabel *bpWindow::getGdaxEthAskLabel() {
    return ui->mGdaxEthAskLabel;
}

QLabel *bpWindow::getGdaxEthBidLabel() {
    return ui->mGdaxEthBidLabel;
}



QLineEdit *bpWindow::getBuySpotAmount() {
    return ui->mBuySpotAmount;
}

QLineEdit *bpWindow::getSellSpotAmount() {
    return ui->mSellSpotAmount;
}

//QFrame *bpWindow::getCbBTCPricePlacer() {
//    return ui->mCbBTCPriceChartPlacer;
//}

//QFrame *bpWindow::getCbLTCPricePlacer() {
//    return ui->mCbLTCPriceChartPlacer;
//}

//QFrame *bpWindow::getCbETHPricePlacer() {
//    return ui->mCbETHPriceChartPlacer;
//}

//QWidget *bpWindow::getChartsTab() {
//    return ui->mChartsTab;
//}

QWidget *bpWindow::getTransactionTab() {
    return ui->mTransactionsTab;
}






coinbaseAccountSetupWindow *bpWindow::getAccSetupWindow() {
    return mCbSetupWin;
}

QComboBox* bpWindow::getAccountsCombo() {
    return ui->mCbAccountComboBox;
}

QComboBox* bpWindow::getDepositFromPayMethodCombo() {
    return ui->mDepFromComboBox;
}

QComboBox* bpWindow::getDepositToPayMethodCombo() {
    return ui->mDepToComboBox;
}

QComboBox* bpWindow::getWithdrawFromPayMethodCombo() {
    return ui->mWithdrawFromComboBox;
}

QComboBox* bpWindow::getWithdrawToPayMethodCombo() {
    return ui->mWithdrawToComboBox;
}

QComboBox* bpWindow::getBuySpotWithPaymentMethodComboBox() {
    return ui->mBuySpotWithPaymentMethodComboBox;
}

QComboBox* bpWindow::getSellSpotWithPaymentMethodComboBox() {
    return ui->mSellSpotWithPaymentMethodComboBox;
}

QComboBox* bpWindow::getBuySpotTypeComboBox() {
    return ui->mBuySpotTypeComboBox;
}

QComboBox* bpWindow::getSellSpotTypeComboBox() {
    return ui->mSellSpotTypeComboBox;
}

QComboBox* bpWindow::getXferFromCbWalletComboBox() {
    return ui->mXferFromCbWalletCombo;
}

QComboBox* bpWindow::getXferToCbWalletComboBox() {
    return ui->mXferToCbWalletCombo;
}

QComboBox* bpWindow::getXferFromCbWalletTargetComboBox() {
    return ui->mXferFromCbWalletTarget;
}

QComboBox* bpWindow::getXferToCbWalletSourceComboBox() {
    return ui->mXferToCbWalletSource;
}

QComboBox* bpWindow::getPlaceGdaxLimitSellTargetComboBox() {
    return ui->mPlaceGdaxLimitSellTarget;
}

QComboBox* bpWindow::getPlaceGdaxLimitBuySourceComboBox() {
    return ui->mPlaceGdaxLimitBuySource;
}


QComboBox* bpWindow::getPlaceGdaxLimitBuyTypeComboBox() {
    return ui->mPlaceLimitBuyType;
}

QComboBox* bpWindow::getPlaceGdaxLimitSellTypeComboBox() {
    return ui->mPlaceLimitSellType;
}


QComboBox* bpWindow::getGdaxAccountComboBox() {
    return ui->mGdaxAccountComboBox;
}

QLineEdit* bpWindow::getDepositFromPayMethodAmount() {
    return ui->mDepositFromAmount;
}

QLineEdit* bpWindow::getWithdrawToPayMethodAmount() {
    return ui->mWithdrawToAmount;
}

QLineEdit* bpWindow::getXferFromCoinbaseAmount() {
    return ui->mXferFromCoinbaseAmount;
}

QLineEdit* bpWindow::getXferToCoinbaseAmount() {
    return ui->mXferToCoinbaseAmount;
}


QLineEdit* bpWindow::getPlaceLimitBuyAmount() {
    return ui->mPlaceLimitBuyAmount;
}

QLineEdit* bpWindow::getPlaceLimitBuyPrice() {
    return ui->mPlaceLimitBuyPrice;
}

//mPlaceLimitSellAmount
QLineEdit* bpWindow::getPlaceLimitSellAmount() {
    return ui->mPlaceLimitSellAmount;
}

QLineEdit* bpWindow::getPlaceLimitSellPrice() {
    return ui->mPlaceLimitSellPrice;
}

QLineEdit* bpWindow::getAutoGdaxMinCryptoBuy() {
    return ui->mAutoGdaxMinCryptoBuy;
}

//getAutoGdaxMinUSDBuy()
QLineEdit* bpWindow::getAutoGdaxMinUSDBuy() {
    return ui->mAutoGdaxMinUSDBuy;
}

QLineEdit* bpWindow::getAutoGdaxMaxUSDBuy() {
    return ui->mAutoGdaxMaxUSDBuy;
}

//getAutoGdaxMinPercentProfit
QLineEdit* bpWindow::getAutoGdaxMinPercentProfit() {
    return ui->mMinProfitPercent;
}


QPushButton* bpWindow::getDepositFromPayMethodButton() {
    return ui->mDepositFromPaymentMethodButton;
}

QPushButton* bpWindow::getWithdrawToPayMethodButton() {
    return ui->mWithdrawToPaymentMethodButton;
}

QPushButton* bpWindow::getQuoteBuySpotForPaymentMethodButton() {
    return ui->mQuoteBuySpotForPaymentMethodButton;
}

QPushButton* bpWindow::getQuoteSellSpotForPaymentMethodButton() {
    return ui->mQuoteSellSpotForPaymentMethodButton;
}

QPushButton* bpWindow::getBuySpotForPaymentMethodButton() {
    return ui->mBuySpotForPaymentMethodButton;
}

QPushButton* bpWindow::getSellSpotForPaymentMethodButton() {
    return ui->mSellSpotForPaymentMethodButton;
}

QPushButton* bpWindow::getRefreshTransactionsButton() {
    return ui->mRefreshTransactionsButton;
}

QPushButton *bpWindow::getRefreshAutoSpotTraderTransactionsButton() {
    return ui->mRefreshAutoSpotTraderTransactionsButton;
}

QPushButton *bpWindow::getXferFromCbWalletButton() {
    return ui->mGdaxTransferFrom;
}

QPushButton *bpWindow::getXferToCbWalletButton() {
    return ui->mGdaxTransferTo;
}

//QCheckBox *bpWindow::getAutoSpotTraderEnabledCheckBox() {
//    return ui->mAutoSpotTradeEnabledCheckBox;
//}

QCheckBox *bpWindow::getAutoRefreshGdaxBalanceEnabledCheckBox() {
    return ui->mAutoRefreshGdaxBalanceCheckBox;
}

QCheckBox *bpWindow::getAutoRefreshCbBalanceEnabledCheckBox() {
    return ui->mAutoRefreshBalanceCheckBox;
}

QCheckBox *bpWindow::getGdaxAutoTradeEnabledCheckBox() {
    return ui->mAutoGDAXTradeEnabledCheckBox;
}

QCheckBox *bpWindow::getWebSocketSubBtcUsd() {
    return ui->mWebSocketSubBtcUsdCheckBox;
}

QCheckBox *bpWindow::getWebSocketSubLtcUsd() {
    return ui->mWebSocketSubLtcUsdCheckBox;
}

QCheckBox *bpWindow::getWebSocketSubEthUsd() {
    return ui->mWebSocketSubEthUsdCheckBox;
}

QCheckBox *bpWindow::getWebSocketSubBchUsd() {
    return ui->mWebSocketSubBchUsdCheckBox;
}


//mAutoGDAXTradeEnabledCheckBox

void bpWindow::killAccSetupWindow() {
    delete mCbSetupWin;
    mCbSetupWin = NULL;
}

void bpWindow::killGdaxAccSetupWindow() {
    delete mGdaxSetupWin;
    mGdaxSetupWin = NULL;
}

///////////
//Slots
///////////
void bpWindow::accountSetupClicked() {
    if ( mCbSetupWin != NULL ) {
        delete mCbSetupWin;
        mCbSetupWin = new coinbaseAccountSetupWindow(this);
    } else {
        mCbSetupWin = new coinbaseAccountSetupWindow(this);
    }
    mCbSetupWin->show();
}

void bpWindow::accountSetupGDAXClicked() {
    if ( mGdaxSetupWin != NULL ) {
        delete mGdaxSetupWin;
        mGdaxSetupWin = new gdaxAccountSetupWindow(this);
    } else {
        mGdaxSetupWin = new gdaxAccountSetupWindow(this);
    }
    mGdaxSetupWin->show();
}

//void bpWindow::on_mAutoSpotTradeEnabledCheckBox_toggled(bool checked) {
//    if ( checked ) {
//        mProphet->say("Enabling Auto Spot Trader.");
//        mProphet->enableAutoSpotTrader();
//    } else {
//        mProphet->say("Disabling Auto Spot Trader.");
//        mProphet->disableAutoSpotTrader();
//    }
//}

void bpWindow::on_mAutoRefreshBalanceCheckBox_toggled(bool checked) {
    if ( checked ) {
        mProphet->say("Enabling AutoRefreshCbAccount.");
        mProphet->enableAutoRefreshCbAccount();
    } else {
        mProphet->say("Disabling AutoRefreshCbAccount.");
        mProphet->disableAutoRefreshCbAccount();
    }
}

void bpWindow::on_mGdaxTransferFrom_released() {
    mProphet->manualGdaxTransferFromClicked();
}

void bpWindow::on_mGdaxTransferTo_released() {
    mProphet->manualGdaxTransferToClicked();
}

void bpWindow::on_mPlaceGdaxLimitBuy_released() {
    mProphet->manualPlaceGdaxLimitBuyClicked();
}

void bpWindow::on_mPlaceGdaxLimitSell_released() {
    mProphet->manualPlaceGdaxLimitSellClicked();
}

void bpWindow::on_mCancelAllGdaxOrders_released() {
    mProphet->cancelAllGdaxOrders();
}

void bpWindow::on_mAutoGDAXTradeEnabledCheckBox_toggled(bool checked) {
    if ( checked ) {
        mProphet->enableGDAXTrader();
    } else {
        mProphet->disableGDAXTrader();
    }
}

void bpWindow::on_pushButton_released() {
    mProphet->mGdaxAutoTradeInstance->autoTradeCheck();
}
