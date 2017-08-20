#include "bpwindow.h"
#include "ui_bpwindow.h"

bpWindow::bpWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::bpWindow), mCbSetupWin(NULL) {
    ui->setupUi(this);
    //bpWindow creates the actual bitProphet
    //bitProphet controls everything
    //bpWindow handles display tasks for bitProphet
    mProphet = new bitProphet(this);

    //Bind Menus
    connect(ui->actionAccount,&QAction::triggered,this, &bpWindow::accountSetupClicked);
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

QTextEdit * bpWindow::getDebugLog() {
    return ui->mDebugLog;
}

QTableWidget *bpWindow::getAccountTablePlacer() {
    return ui->cbAccountTablePlacer;
}

QWidget *bpWindow::getAccountsTab() {
    return ui->mCoinbaseTab;
}

QLabel *bpWindow::getAccountsNativeBalanceTotalLabel() {
    return ui->mTotalNativeValue;
}

QLabel *bpWindow::getBtcSpotPriceLabel() {
    return ui->mBtcSpotPriceLabel;
}

QLabel *bpWindow::getLtcSpotPriceLabel() {
    return ui->mLtcSpotPriceLabel;
}

QLabel *bpWindow::getEthSpotPriceLabel() {
    return ui->mEthSpotPriceLabel;
}

QFrame *bpWindow::getCbBTCPricePlacer() {
    return ui->mCbBTCPriceChartPlacer;
}

QFrame *bpWindow::getCbLTCPricePlacer() {
    return ui->mCbLTCPriceChartPlacer;
}

QFrame *bpWindow::getCbETHPricePlacer() {
    return ui->mCbETHPriceChartPlacer;
}

QWidget *bpWindow::getChartsTab() {
    return ui->mChartsTab;
}

coinbaseAccountSetupWindow *bpWindow::getAccSetupWindow() {
    return mCbSetupWin;
}

QComboBox* bpWindow::getAccountsCombo() {
    return ui->mCbAccountComboBox;
}

void bpWindow::killAccSetupWindow() {
    delete mCbSetupWin;
    mCbSetupWin = NULL;
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
