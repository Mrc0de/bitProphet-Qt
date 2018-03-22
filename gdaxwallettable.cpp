#include "gdaxwallettable.h"

gdaxWalletTable::gdaxWalletTable(gdaxAccount *acc,gdaxApiHandler *parent) : QObject(parent), mTableWidget(NULL) {
    mTableWidget = new QTableWidget(acc->getWalletCount(),6, parent->mParent->mParent->getGdaxAccountsTab() );
    mParent = parent;
    QWidget *placer = mParent->mParent->mParent->getGdaxAccountTablePlacer();
    mTableWidget->setGeometry( placer->geometry() );
    placer->hide(); placer = NULL; //dont delete it, just grab and hide, repeat
    mTableWidget->show();
    for (int w=0;w<acc->getWalletCount();w++){
        addWallet(acc->getWallet(w),w);
    }
}

gdaxWalletTable::~gdaxWalletTable() {
    //mParent->say("[gdaxWalletTable] gdaxWalletTable Destroyed." );
    if (mTableWidget != NULL ) { delete mTableWidget; }
}

void gdaxWalletTable::addWallet(gdaxWallet *w, int r) {
    int row = r; int column = 0;
    QTableWidgetItem *newItem = new QTableWidgetItem(w->mCurrency);
    mTableWidget->setItem(row, column, newItem);
    mTableWidget->setHorizontalHeaderItem(column,new QTableWidgetItem("Currency"));
    newItem = new QTableWidgetItem(w->mBalance);
    mTableWidget->setItem(row, column+1, newItem);
    mTableWidget->setHorizontalHeaderItem(column+1,new QTableWidgetItem("Balance"));
    newItem = new QTableWidgetItem(w->mAvailable);
    mTableWidget->setItem(row, column+2, newItem);
    mTableWidget->setHorizontalHeaderItem(column+2,new QTableWidgetItem("Available"));
    newItem = new QTableWidgetItem(w->mHold);
    mTableWidget->setItem(row, column+3, newItem);
    mTableWidget->setHorizontalHeaderItem(column+3,new QTableWidgetItem("Hold"));
    newItem = new QTableWidgetItem(w->mId.mid(0,8)); //never use values from this table
    // only use values taken directly from the account instance, these are DISPLAY models
    mTableWidget->setItem(row, column+4, newItem);
    mTableWidget->setHorizontalHeaderItem(column+4,new QTableWidgetItem("Id"));
    newItem = new QTableWidgetItem(w->mProfileId.mid(0,8));
    mTableWidget->setItem(row, column+5, newItem);
    mTableWidget->setHorizontalHeaderItem(column+5,new QTableWidgetItem("ProId"));
}
