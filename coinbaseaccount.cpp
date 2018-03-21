#include "coinbaseaccount.h"

coinbaseAccount::coinbaseAccount(cbApiHandler *parent) {
    mParent = parent;
    mPtrName = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    say("coinbaseAccount Instance at " + mPtrName);
    mWalletList.clear();
    mPayMethods.clear();
}

coinbaseAccount::~coinbaseAccount() {
    say( "coinbaseAccount Fading...");
}

////////////
// Methods
////////////
void coinbaseAccount::say(QString sayThis) {
    mParent->say( "[coinbaseAccount] " + sayThis);
}


// Wallet List Methods
int coinbaseAccount::addWallet() {
    coinbaseWallet *add = new coinbaseWallet(this);
    mWalletList.append(add);
    return mWalletList.count()-1; //returns the index of the newly added wallet for reference
}

coinbaseWallet* coinbaseAccount::getWallet(int index) {
    return mWalletList.at(index);
}

int coinbaseAccount::getWalletCount() {
    return mWalletList.count();
}

void coinbaseAccount::clearWallets() {
    mWalletList.clear();
}

int coinbaseAccount::addPaymentMethod() {
    coinbasePaymentMethod *add = new coinbasePaymentMethod(this);
    mPayMethods.append(add);
    return mPayMethods.count()-1; //returns the index of the newly added ... for reference
}

coinbasePaymentMethod* coinbaseAccount::getPaymentMethod(int index) {
    return mPayMethods.at(index);
}




int coinbaseAccount::getPaymentMethodCount() {
    return mPayMethods.count();
}

/////////
// Slots
/////////
