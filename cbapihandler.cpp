#include "cbapihandler.h"

cbApiHandler::cbApiHandler(QObject *parent) : QObject(parent),mAccount(NULL), mWalletTableWidget(NULL),mTransactionTable(NULL) {
    mParentProphet = reinterpret_cast<bitProphet*>(parent);
    mPtrName = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    say("Api Handler Created at " + mPtrName);
    mParentProphet->mParent->getCoinbaseTabLog()->document()->setMaximumBlockCount(256);
    if ( mParentProphet->getDb()->getAccountList().length() > 0 ) {
        QString defCbId = mParentProphet->getDb()->getDefaultAccountId();
        if ( defCbId != "0" ) {
            say("Found Default Account. [" + defCbId +"]");
//            mParentProphet->mParent->getAutoSpotTraderEnabledCheckBox()->setEnabled(true);
            mParentProphet->mParent->getAutoRefreshCbBalanceEnabledCheckBox()->setEnabled(true);
            mAccount = new coinbaseAccount(this);
            //Load Default Account
            mParentProphet->getDb()->loadAccountById(mAccount,defCbId);
            //Load accounts tab dropdown with accounts
            QList<QString> daList= mParentProphet->getDb()->getAccountList();
            for ( int c=0;c<daList.length();c++) {
                mParentProphet->addAccountToCoinbaseComboBox(daList.at(c));
            }
            //Check balance once
            listAccounts();
            //I dont know why i created the transactionTable here. Move it.
            mTransactionTable = new cbTransactionTable(this);
            //Check for Payment Methods linked to account (Payment methods are used to fund cb and gdax accounts and to withdraw #winnings)
            if ( mAccount->getPaymentMethodCount() < 1 ) {
                say("No Payment Methods Found!");
                say("Fetching Payment Methods from default account [" + defCbId +"]");
                //hrmm...
                listPaymentMethods(); //doh
            }
            //say( "KeyLen: " + QString().setNum(mAccount->mApiKey.length()) );
           // say( "SecLen: " + QString().setNum(mAccount->mApiSecret.length()) );
            //Start Spot Check            
            if ( mParentProphet->mAutoCheckSpotPrices ) {
                QTimer::singleShot(1000,this,SLOT(fetchSpotPrices()));
            }
            //start Auto Simple Trading (if)

        } else {
            say("Default Account was not found.");
            say("Create one using the Setup Menu.");
        }
        say("Api Handler Loaded.");
    }
}




cbApiHandler::~cbApiHandler() {
    if (mTransactionTable != NULL ) { delete mTransactionTable; }
    if (mAccount != NULL ) { delete mAccount; }
    say("Api Handler Fading...");
}

///////////
// Methods
///////////

void cbApiHandler::say(QString sayThis) {
    mParentProphet->say( "[cbApiHandler] " + sayThis);
}

QString cbApiHandler::getCoinbaseApiSecret() {
    //Return from whichever account is active (one at time)
    return mAccount->mApiSecret;
}

QString cbApiHandler::getCoinbaseApiKey() {
    //Return from whichever account is active (one at time)
    return mAccount->mApiKey;
}

void cbApiHandler::listPaymentMethods() {
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    //                                                            //
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    // URL: https://api.coinbase.com/v2/payment-methods
    // method: GET
    // requestPath: /v2/payment-methods
    // body: EMPTY ie: ""
    mParentProphet->setProphetState("FETCH");
    //Creating a new coinbaseApiRequest
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");        //list accounts is a GET
    req->setPath("/v2/payment-methods");  //the url path,etc..
    req->setBody("");             //no body needed (for this one)
    req->setType("listPaymentMethods");//just for us, forgot why...
    req->sendRequest();           //sendRequest has the info/access it needs to do the rest.
    return;
}

///////////////////////
// RESPONSE PROCESSORS
///////////////////////

void cbApiHandler::processResponse( cbApiResponse *resp ) {
    QString type = resp->getType();
    // say("Processing Response Type: " + type);
    if (type == "listAccounts" ) {
        listAccountProcessResponse(resp);
        if ( mParentProphet->mAutoRefreshAccount ) {
            int timer= mParentProphet->mAutoRefreshAccountInterval;
            QTimer::singleShot(timer, mParentProphet, SLOT(listAccountSlot()));
        }
    } else if (type == "listPaymentMethods" ) {
          listPayMethodProcessResponse(resp);
    }  else if (type == "buySpotQuote" ) {
          processBuySpotQuoteResponse(resp);
    } else if (type == "sellSpotQuote" ) {
          processSellSpotQuoteResponse(resp);
    } else if (type == "buySpot" ) {
          processBuySpotResponse(resp);
    } else if (type == "sellSpot" ) {
          processSellSpotResponse(resp);
    } else if (type == "fetchTransactions" ) {
          processFetchTransactionsResponse(resp);
    } else if (type == "buyAutoSpot" ) {
        processBuyAutoSpotResponse(resp);
    } else {
        say("Unknown Response Type: " + type);
    }
    // say("Done Processing Response Type: " + type);
    resp->getParent()->deleteLater();
    mParentProphet->mParent->getCbStatProcessedLabel()->setText(QString().setNum(mParentProphet->mParent->getCbStatProcessedLabel()->text().toInt() + 1));
}



void cbApiHandler::processFetchTransactionsResponse(cbApiResponse *resp) {
    QJsonObject obj = *(resp->getResponseContent());
    QJsonArray data  = obj["data"].toArray();
    for (int d=0;d<data.count();d++) {
        //each object is a transaction here
        cbTransaction aTxn(this);
        QJsonObject theObj = data.at(d).toObject();
        QJsonObject theAmount = theObj["amount"].toObject();
        QJsonObject theNative = theObj["native_amount"].toObject();
        if ( theAmount["currency"].toString() == "USD" ) { continue; }
        aTxn.mType = theObj["type"].toString();
        aTxn.mAmountObj = theAmount;
        aTxn.mAmount = theAmount["amount"].toString();
        aTxn.mNativeAmountObj = theNative;
        aTxn.mAmountNative = theNative["amount"].toString();
        aTxn.mId = theObj["id"].toString();
        aTxn.mCreatedAt = theObj["created_at"].toString();
        aTxn.mDescription = theObj["description"].toString();
        mTransactionTable->addTransaction(&aTxn,d);
    }
    //todo check pagination for next_uri != "null"
}

void cbApiHandler::processBuyAutoSpotResponse(cbApiResponse *resp) {
    QJsonObject obj = *(resp->getResponseContent());
    QJsonObject data  = obj["data"].toObject();
    QJsonArray fees  = data["fees"].toArray();
    QJsonObject theAmount = data["amount"].toObject();
    QString amountBought = theAmount["amount"].toString();
    QString coinBought = theAmount["currency"].toString();
    QJsonObject theTotal = data["total"].toObject();
    QString amountPaid = theTotal["amount"].toString();
    mParentProphet->mAutoSpot->say("################ ",coinBought);
    mParentProphet->mAutoSpot->say("# Buy Response #",coinBought);
    mParentProphet->mAutoSpot->say("# Bought " + coinBought, coinBought);
    mParentProphet->mAutoSpot->say("# Qty: " + amountBought, coinBought);
    mParentProphet->mAutoSpot->say("# Paid: $" + amountPaid, coinBought);
    mParentProphet->mAutoSpot->say("################ ",coinBought);
    mParentProphet->mAutoSpot->say("",coinBought);
    mParentProphet->getDb()->insertAutoSpotTrade(coinBought,"BUY",amountPaid,"0.00","BOUGHT",amountBought);
}

void cbApiHandler::processBuySpotResponse( cbApiResponse *resp ) {
    //{"data":{"
        //id":"5f7b3475","status":"created","payment_method":
            // {"id":"3fe1a18c","resource":"payment_method","resource_path":"/v2/payment-methods/3fe1a18c"},
            // "transaction":null,"user_reference":"36T4PRJ8","created_at":"2017-08-22T02:23:40Z","updated_at":"2017-08-22T02:23:41Z","resource":"buy",
            //"resource_path":"/v2/accounts/65d20557/buys/5f7b3475",
            //"fees":[{"type":"coinbase","amount":{"amount":"0.84","currency":"USD"}},{"type":"bank","amount":{"amount":"0.15","currency":"USD"}}],
            //"amount":{"amount":"0.00988747","currency":"ETH"},"total":{"amount":"4.14","currency":"USD"},"subtotal":{"amount":"3.15","currency":"USD"},"committed":true,
            //"payout_at":"2017-08-27T02:23:40Z","instant":false,"requires_completion_step":false}}
    cbTabLog("### BUY Response ###");
    QJsonObject obj = *(resp->getResponseContent());
    QJsonObject data  = obj["data"].toObject();
    QJsonArray fees  = data["fees"].toArray();
    QJsonObject cbFee = fees.at(0).toObject();
    cbFee = cbFee["amount"].toObject();
    QString cbFeeStr = cbFee["amount"].toString();
    cbTabLog("# Base Fee: " + cbFeeStr);
    QJsonObject bankFee = fees.at(1).toObject();
    bankFee = bankFee["amount"].toObject();
    QString bankFeeStr = bankFee["amount"].toString();
    if ( bankFeeStr != "0.00" ) {
        cbFeeStr = QString().setNum(cbFeeStr.toDouble() + bankFeeStr.toDouble());
    }
    cbTabLog("# Bank Fee: " + bankFeeStr);
    QJsonObject paging  = obj["pagination"].toObject();
    QJsonObject amountData = data["amount"].toObject();
    QJsonObject totalData = data["total"].toObject();
    cbTabLog( "# Buying " + amountData["amount"].toString());
    cbTabLog( "# Actual Fee: " + cbFeeStr);
    cbTabLog( "# Total $" + totalData["amount"].toString());
    cbTabLog( "# Payout @ " + data["payout_at"].toString());
    cbTabLog("################## ");
    cbTabLog(" ");
}

void cbApiHandler::processSellSpotResponse( cbApiResponse *resp ) {
    cbTabLog("### SELL Response ###");
    QJsonObject obj = *(resp->getResponseContent());
    QJsonObject data  = obj["data"].toObject();
    QJsonArray fees  = data["fees"].toArray();
    QJsonObject cbFee = fees.at(0).toObject();
    cbFee = cbFee["amount"].toObject();
    QString cbFeeStr = cbFee["amount"].toString();
    cbTabLog("# Base Fee: " + cbFeeStr);
    QJsonObject bankFee = fees.at(1).toObject();
    bankFee = bankFee["amount"].toObject();
    QString bankFeeStr = bankFee["amount"].toString();
    if ( bankFeeStr != "0.00" ) {
        cbFeeStr = QString().setNum(cbFeeStr.toDouble() + bankFeeStr.toDouble());
    }
    cbTabLog("# Bank Fee: " + bankFeeStr);
    QJsonObject paging  = obj["pagination"].toObject();
    QJsonObject amountData = data["amount"].toObject();
    QJsonObject totalData = data["total"].toObject();
    cbTabLog( "# Selling " + amountData["amount"].toString());
    cbTabLog( "# Actual Fee: " + cbFeeStr);
    cbTabLog( "# Total $" + totalData["amount"].toString());
    cbTabLog("################## ");
    cbTabLog(" ");
}

void cbApiHandler::processBuySpotQuoteResponse( cbApiResponse *resp ) {
    cbTabLog("### BUY Quote Response ###");
    QJsonObject obj = *(resp->getResponseContent());
    QJsonObject data  = obj["data"].toObject();
    QJsonArray fees  = data["fees"].toArray();
    QJsonObject cbFee = fees.at(0).toObject();
    cbFee = cbFee["amount"].toObject();
    QString cbFeeStr = cbFee["amount"].toString();
    cbTabLog("# Base Fee: " + cbFeeStr);
    QJsonObject bankFee = fees.at(1).toObject();
    bankFee = bankFee["amount"].toObject();
    QString bankFeeStr = bankFee["amount"].toString();
    if ( bankFeeStr != "0.00" ) {
        cbFeeStr = QString().setNum(cbFeeStr.toDouble() + bankFeeStr.toDouble());
    }
    cbTabLog("# Bank Fee: " + bankFeeStr);
    QJsonObject paging  = obj["pagination"].toObject();
    QJsonObject amountData = data["amount"].toObject();
    QJsonObject totalData = data["total"].toObject();
    cbTabLog( "# Buying " + amountData["amount"].toString());
    cbTabLog( "# Actual Fee: " + cbFeeStr);
    cbTabLog( "# Total $" + totalData["amount"].toString());
    mParentProphet->mParent->getBuySpotAmount()->setText(amountData["amount"].toString());
    mParentProphet->mParent->getBuySpotFeeLabel()->setText(cbFeeStr);
    mParentProphet->mParent->getBuySpotTotalLabel()->setText(totalData["amount"].toString());
    cbTabLog("################## ");
    cbTabLog(" ");
    mParentProphet->mParent->getBuySpotForPaymentMethodButton()->setEnabled(1);
    QTimer::singleShot(5000,this,SLOT(disableBuySpotButton())); //disable button and force re-quote after 5 seconds
}



void cbApiHandler::processSellSpotQuoteResponse( cbApiResponse *resp ) {
    cbTabLog("### SELL Quote Response ###");
    QJsonObject obj = *(resp->getResponseContent());
    QJsonObject data  = obj["data"].toObject();

    QString soldCoinBalance("0.00");
    for ( int a=0;a<mAccount->getWalletCount();a++ ) {
        if ( mAccount->getWallet(a)->mCurrency == data["amount"].toObject()["currency"].toString() ) {
            soldCoinBalance =  mAccount->getWallet(a)->mAmount;
        }
    }

    QJsonArray fees  = data["fees"].toArray();
    QJsonObject amount  = data["amount"].toObject();
    QJsonObject cbFee = fees.at(0).toObject();
    cbFee = cbFee["amount"].toObject();
    QString cbFeeStr = cbFee["amount"].toString();
    cbTabLog("# Base Fee: " + cbFeeStr);
    QJsonObject bankFee = fees.at(1).toObject();
    bankFee = bankFee["amount"].toObject();
    QString bankFeeStr = bankFee["amount"].toString();
    if ( bankFeeStr != "0.00" ) {
        cbFeeStr = QString().setNum(cbFeeStr.toDouble() + bankFeeStr.toDouble());
    }
    cbTabLog("# Bank Fee: " + bankFeeStr);

    QJsonObject paging  = obj["pagination"].toObject();

    QJsonObject totalData = data["total"].toObject();
    QJsonObject subTotalData = data["subtotal"].toObject();
    cbTabLog( "# Actual Fee: " + cbFeeStr);    
    cbTabLog( "# subTotal " + subTotalData["amount"].toString());
    cbTabLog( "# Total " + totalData["amount"].toString());
    cbTabLog( "# Amount " + data["amount"].toObject()["amount"].toString());
    mParentProphet->mParent->getSellSpotAmount()->setText(amount["amount"].toString());
    mParentProphet->mParent->getSellSpotFeeLabel()->setText(cbFeeStr);
    mParentProphet->mParent->getSellSpotTotalLabel()->setText(totalData["amount"].toString());
    cbTabLog("################## ");
    cbTabLog(" ");
    mParentProphet->mParent->getSellSpotForPaymentMethodButton()->setEnabled(1);
    QTimer::singleShot(20000,this,SLOT(disableSellSpotButton())); //disable button and force re-quote after 20 seconds
}

void cbApiHandler::processBadListAccountsResponse() {
    if ( mParentProphet->mAutoRefreshAccount ) {
        say("Restarting Timer [autoRefreshAccount]");
        int timer= mParentProphet->mAutoRefreshAccountInterval;
        QTimer::singleShot(timer, mParentProphet, SLOT(listAccountSlot()));
    }
}

void cbApiHandler::listAccountProcessResponse(cbApiResponse *resp) {
        if ( mAccount->getWalletCount() > 0 ) {
            for ( int a=0;a<mAccount->getWalletCount();a++) {
                mAccount->getWallet(a)->deleteLater();
            }
            mAccount->clearWallets();
        }
        QJsonObject obj = *(resp->getResponseContent());
        QJsonArray data  = obj["data"].toArray();
        QJsonObject paging  = obj["pagination"].toObject();
        //say ( "Found Data: " + QString().setNum(data.count()) + " Entries Found.");
        for (int d=0;d<data.count();d++) {
            // each element here is a wallet each of which has:
            // id , name, primary(bool), type, currency,
            // balance(obj), native_balance(obj), created_at, updated_at, resource, resource_path, ready(bool)
            QJsonObject el = data.at(d).toObject();
            //          say("ID: " + el["id"].toString().mid(0,el["id"].toString().indexOf('-') ) );
            //          say("Name: " + el["name"].toString() );
            //          say("Type: " + el["type"].toString() );
            //          say("Currency: " + el["currency"].toString() );
            QJsonObject balEl = el["balance"].toObject();
            //          say("Balance Amount: " + balEl["amount"].toString() );
            //          say("Currency Type: " + balEl["currency"].toString() );
            QJsonObject natBalEl = el["native_balance"].toObject();
            //          say("Native Balance Amount: $" + natBalEl["amount"].toString() );
            //          say("Native Currency Type: " + natBalEl["currency"].toString() );
            //          say("Resource: " + el["resource"].toString() );
            //          //say("Ready?: " + QString().setNum(el["ready"].toInt()) );  //API DOCS says this exists, it does not.
            //          say("------------------------------------------");

            // COFFEEEEEEEEE

            int newWalletIndex = mAccount->addWallet();
            coinbaseWallet *newWallet = mAccount->getWallet(newWalletIndex);
            newWallet->mId = el["id"].toString();
            newWallet->mName = el["name"].toString();
            newWallet->mType = el["type"].toString();
            newWallet->mCurrency = balEl["currency"].toString();
            newWallet->mAmount = balEl["amount"].toString();
            newWallet->mCurrencyNative = natBalEl["currency"].toString();
            newWallet->mAmountNative = natBalEl["amount"].toString();
            newWallet->mResource = el["resource"].toString();
            newWallet->mCreated = el["created_at"].toString();
            newWallet->mUpdated = el["updated_at"].toString();
            //say ("Added Wallet " + el["id"].toString().mid(0,el["id"].toString().indexOf('-') ));
            if ( mWalletTableWidget != NULL ) {
                mWalletTableWidget->deleteLater();
            }
            mWalletTableWidget = new cbWalletTable(mAccount,this);
        }
        //say ( "Found Paging: " + QString().setNum(paging.count()) + " Entries Found.");
}



void cbApiHandler::listPayMethodProcessResponse( cbApiResponse *resp ) {
    QJsonObject *r = resp->getResponseContent();
    QStringList keys = r->keys();
    int k;
    for(k=0;k<keys.count();k++) {
        //say("rKey " + QString().setNum(k) + ": " + QString(keys.at(k)) );
        if(keys.at(k) == "data") {
            //say("Got It...");
            break;
        }
    }
    QString key=keys.at(k);
    QJsonArray data = r->value(key).toArray();
    //say("Loading PMs From Response...");
    for(k=0;k<data.count();k++) {
        if ( data.at(k).isObject() ) {
            //Each one of these is a Payment Method
            int newPayMethod = mAccount->addPaymentMethod();
            //say("Found PM #" +  QString().setNum(newPayMethod));
            //populate pm
            QJsonObject pmList = data.at(k).toObject();
            keys = pmList.keys();
            for (int pm=0;pm<keys.count();pm++){
                if ( keys.at(pm) == "id" ) {
                    mAccount->getPaymentMethod(newPayMethod)->mId = pmList.value(keys.at(pm)).toString();
                    //say("PM ID -> " + pmList.value(keys.at(pm)).toString().mid(0,pmList.value(keys.at(pm)).toString().indexOf('-')));
                } else if ( keys.at(pm) == "currency" ) {
                    mAccount->getPaymentMethod(newPayMethod)->mCurrency = pmList.value(keys.at(pm)).toString();
                    //say("PM CURRENCY -> " + pmList.value(keys.at(pm)).toString());
                } else if ( keys.at(pm) == "name" ) {
                    mAccount->getPaymentMethod(newPayMethod)->mName = pmList.value(keys.at(pm)).toString();
                    //say("PM NAME -> " + pmList.value(keys.at(pm)).toString());
                }  else if ( keys.at(pm) == "type" ) {
                    mAccount->getPaymentMethod(newPayMethod)->mType = pmList.value(keys.at(pm)).toString();
                    //say("PM TYPE -> " + pmList.value(keys.at(pm)).toString());
                } else if ( keys.at(pm) == "fiat_account" ) {
                    QJsonObject fiat = pmList.value(keys.at(pm)).toObject();
                    QStringList fKeys = fiat.keys();
                    for (int fk=0;fk<fKeys.count();fk++) {
                        if ( fKeys.at(fk) == "resource_path" ) {
                            mAccount->getPaymentMethod(newPayMethod)->mFiatAccountResourcePath = fiat.value(fKeys.at(fk)).toString();
                            //say("PM FIAT REPATH -> " + fiat.value(fKeys.at(fk)).toString());
                        } else if ( fKeys.at(fk) == "id" ) {
                            mAccount->getPaymentMethod(newPayMethod)->mFiatAccountId = fiat.value(fKeys.at(fk)).toString();
                            //say("PM FIAT AccId -> " + fiat.value(fKeys.at(fk)).toString());
                        }
                    }
                }
            }
        }
    }
    //resp->printResponse();
    ///////////////////////////
    /// Next, We need to load our payment methods into the accounts page deposit and withdraw combo boxes
    /// Deposits -> From account can be ach_bank_account, more as I find them.
    /// ----------> To Accounts can be fiat_account, mare added if needed (its your USD wallet)
    ///
    /// Withdrawals -> From account can be type fiat_account, more added when found (its your USD wallet(USD vault,etc))
    /// ----------> To Accounts can be type ach_bank_account, more as I find them.
    ///////////////////////////
    QComboBox *depFrom = mParentProphet->mParent->getDepositFromPayMethodCombo();
    QComboBox *depTo = mParentProphet->mParent->getDepositToPayMethodCombo();
    QComboBox *withdrawFrom = mParentProphet->mParent->getWithdrawFromPayMethodCombo();
    QComboBox *withdrawTo = mParentProphet->mParent->getWithdrawToPayMethodCombo();
    QComboBox *buySpotWith = mParentProphet->mParent->getBuySpotWithPaymentMethodComboBox();
    QComboBox *SellSpotWith = mParentProphet->mParent->getSellSpotWithPaymentMethodComboBox();
    int pmNum = mAccount->getPaymentMethodCount();
    for (int a=0;a<pmNum;a++) {
        QString nickName = mAccount->getPaymentMethod(a)->mCurrency + " " + mAccount->getPaymentMethod(a)->mName.right(6) + " [" + mAccount->getPaymentMethod(a)->mType + "] "
                + "(" + mAccount->getPaymentMethod(a)->mId.mid(0,8) + ")";
        if ( mAccount->getPaymentMethod(a)->mType == "ach_bank_account" ) {
            //add to DepFrom and WithdrawTo list
            depFrom->addItem(nickName);
            depFrom->setItemData(depFrom->findText(nickName),mAccount->getPaymentMethod(a)->mId);
            depFrom->setCurrentIndex(depFrom->findText(nickName));
            withdrawTo->addItem(nickName);
            withdrawTo->setItemData(withdrawTo->findText(nickName),mAccount->getPaymentMethod(a)->mId);
            withdrawTo->setCurrentIndex(withdrawTo->findText(nickName));
        } else if ( mAccount->getPaymentMethod(a)->mType == "fiat_account" ) {
            //add to DepTo and WithdrawFrom list
            depTo->addItem(nickName);
            depTo->setItemData(depTo->findText(nickName),mAccount->getPaymentMethod(a)->mFiatAccountId);
            depTo->setCurrentIndex(depTo->findText(nickName));
            withdrawFrom->addItem(nickName);
            withdrawFrom->setItemData(withdrawFrom->findText(nickName),mAccount->getPaymentMethod(a)->mFiatAccountId);
            withdrawFrom->setCurrentIndex(withdrawFrom->findText(nickName));
        }
        buySpotWith->addItem(nickName);
        buySpotWith->setItemData(buySpotWith->findText(nickName),mAccount->getPaymentMethod(a)->mId);
        buySpotWith->setCurrentIndex(buySpotWith->findText(nickName));

        SellSpotWith->addItem(nickName);
        SellSpotWith->setItemData(SellSpotWith->findText(nickName),mAccount->getPaymentMethod(a)->mId);
        SellSpotWith->setCurrentIndex(SellSpotWith->findText(nickName));

    }
//    // Set signal onto button so it works....
    QPushButton *depFromButton = mParentProphet->mParent->getDepositFromPayMethodButton();
    QPushButton *withToButton = mParentProphet->mParent->getWithdrawToPayMethodButton();
    QPushButton *QuoteBuySpotButton = mParentProphet->mParent->getQuoteBuySpotForPaymentMethodButton();
    QPushButton *QuoteSellSpotButton = mParentProphet->mParent->getQuoteSellSpotForPaymentMethodButton();
    QPushButton *buySpotButton = mParentProphet->mParent->getBuySpotForPaymentMethodButton();
    QPushButton *sellSpotButton = mParentProphet->mParent->getSellSpotForPaymentMethodButton();
    QPushButton *refreshTransactions = mParentProphet->mParent->getRefreshTransactionsButton();
    connect(depFromButton, SIGNAL(clicked(bool)),this,SLOT(depositFromButtonSlot()));
    connect(withToButton, SIGNAL(clicked(bool)),this,SLOT(withdrawToButtonSlot()));
//    connect(QuoteBuySpotButton, SIGNAL(clicked(bool)),this,SLOT( QuoteBuySpotClicked()  ));
//    connect(QuoteSellSpotButton, SIGNAL(clicked(bool)),this,SLOT( QuoteSellSpotClicked() ));
//    connect(sellSpotButton, SIGNAL(clicked(bool)),this,SLOT( sellSpotClicked() ));
//    connect(buySpotButton, SIGNAL(clicked(bool)),this,SLOT( buySpotClicked() ));
    connect(refreshTransactions, SIGNAL(clicked(bool)),this,SLOT( fetchTransactions() ) );
    depFromButton->setEnabled(1);
    withToButton->setEnabled(1);
    QuoteBuySpotButton->setEnabled(1);
    QuoteSellSpotButton->setEnabled(1);
    refreshTransactions->setEnabled(1);
    say("Payment Methods Loaded...");
    say("Manual Withdraw/Deposit/Buy/Sell is Enabled!");
}

///////////
// Slots
///////////

void cbApiHandler::fetchTransactions() {
    if ( mTransactionTable != NULL ) { mTransactionTable->deleteLater(); }
    mTransactionTable = new cbTransactionTable(this);
    // Default, show all Transaction Types
    mParentProphet->setProphetState("FETCH");
    //Instead of manual, Creating a new coinbaseApiRequest
    for (int f=0;f<mAccount->getWalletCount();f++) {
        say("Fetching Transactions [" + mAccount->getWallet(f)->mName + "]");
        cbApiRequest* req = new cbApiRequest(this);
        //mCurrentRequestList.append(req);
        req->setMethod("GET");        //list accounts is a GET
        req->setPath("/v2/accounts/" + mAccount->getWallet(f)->mId + "/transactions" );  //the url path,etc..
        req->setBody("");             //no body needed (for this one)
        req->setType("fetchTransactions");//just for us, forgot why...
        //say("Sending Request...");
        req->sendRequest();
    }
}

void cbApiHandler::disableBuySpotButton() {
    mParentProphet->mParent->getBuySpotForPaymentMethodButton()->setEnabled(0);
}

void cbApiHandler::disableSellSpotButton() {
    mParentProphet->mParent->getSellSpotForPaymentMethodButton()->setEnabled(0);
}

void cbApiHandler::buyAutoSpot(QString total,QString coin) {
    say("Total: " + total);
    QString paidWith("USD");
    say("paidWith: " + paidWith);
    // Payment Method will always be USD fiat_account for AutoSpot Buys
    QString paymentMethod;
    for ( int c=0;c<mAccount->getPaymentMethodCount();c++ ){
        if ( mAccount->getPaymentMethod(c)->mCurrency == paidWith && mAccount->getPaymentMethod(c)->mType == "fiat_account") {
            paymentMethod = mAccount->getPaymentMethod(c)->mId;
        }
    }

    QString reqBody = "{ \"total\": \"" + total + "\", \"currency\": \"" + paidWith + "\", \"payment_method\": \"" + paymentMethod + "\" , \"commit\": true }";
    //say("## rBody -> " + reqBody);
    cbTabLog("#auto Sending Total: " + total);
    cbTabLog("#auto Sending Currency: " + paidWith);
    cbTabLog("#auto Sending commit: TRUE");
    //Creating a new coinbaseApiRequest
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("POST");
    // We need the id of the account the buy will GO TO
    // ie: BTC buy = BTC wallet
    // find selected wallet and get id for URL
    QString destAccount;
    for ( int c=0;c<mAccount->getWalletCount();c++ ){
        say("Wallet Currency: " + mAccount->getWallet(c)->mCurrency );
        if ( mAccount->getWallet(c)->mCurrency == coin ) {
            destAccount = mAccount->getWallet(c)->mId;
        }
    }
    //say("Dest: "+destAccount );
    req->setPath("/v2/accounts/" + destAccount + "/buys");
    req->setBody(reqBody);
    req->setType("buyAutoSpot"); //just for us
    req->sendRequest();
    cbTabLog("# AutoBuy Requested");
}

//void cbApiHandler::buySpotClicked() {
//    cbTabLog("### BUY @ SPOT PRICE ### ");
//    QString buyAmount = mParentProphet->mParent->getBuySpotAmount()->text();
//    cbTabLog("# Buying " + buyAmount + " " + mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() );
//    QString spot;
//    if ( mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() == "BTC" ) {
//        spot = mParentProphet->mParent->getBtcSpotBuyPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() == "LTC" ) {
//        spot = mParentProphet->mParent->getLtcSpotBuyPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() == "ETH" ) {
//        spot = mParentProphet->mParent->getEthSpotBuyPriceLabel()->text();
//    }
//    cbTabLog("# Spot Price $" + spot);
//    QString subTotal = QString().setNum(mParentProphet->mParent->getBuySpotTotalLabel()->text().toDouble() - mParentProphet->mParent->getBuySpotFeeLabel()->text().toDouble());
//    subTotal = trimPriceStringDecimal(subTotal);
//    cbTabLog("# Sub-Total: $" + subTotal );
//    QString fee = mParentProphet->mParent->getBuySpotFeeLabel()->text();
//    fee = trimPriceStringDecimal(fee);
//    if ( fee.toDouble() < 0.99 ) {
//        fee = "0.99";
//    }
//    cbTabLog("# Actual Buy Fee: $" + fee );
//    QString totalBuyAmountAfterFee = QString().setNum(fee.toDouble() + subTotal.toDouble());

//    totalBuyAmountAfterFee = trimPriceStringDecimal(totalBuyAmountAfterFee);
//    cbTabLog("# Actual Buy Total: $" + totalBuyAmountAfterFee );
//    cbTabLog("# Placing Buy");
//    // Send Request
//    QComboBox *buySpotWith = mParentProphet->mParent->getBuySpotWithPaymentMethodComboBox();
//    QString selectedPayment = buySpotWith->currentData().toString();
//    if ( selectedPayment.length() > 0 ) {
//        QString currencyBought = mParentProphet->mParent->getBuySpotTypeComboBox()->currentText();
//        QString currencyPaidWith = mParentProphet->mParent->getBuySpotWithPaymentMethodComboBox()->currentText().mid(0,3);
//        QString accId = selectedPayment;
//        QString reqBody = "{ \"total\": \"" + totalBuyAmountAfterFee + "\", \"currency\": \"" + currencyPaidWith + "\", \"payment_method\": \"" + selectedPayment + "\" , \"commit\": true }";
//        //say("## rBody -> " + reqBody);
//        cbTabLog("# Sending TOTAL: " + totalBuyAmountAfterFee);
//        cbTabLog("# Sending Currency: " + currencyPaidWith);
//        cbTabLog("# Sending commit: TRUE");
//        //cbTabLog("# Sending pm: " + selectedPayment);
//        //Creating a new coinbaseApiRequest
//        cbApiRequest* req = new cbApiRequest(this);
//        req->setMethod("POST");
//        // We need the id of the account the buy will GO TO
//        // ie: BTC buy = BTC wallet
//        // find selected wallet and get id for URL
//        QString destAccount;
//        for ( int c=0;c<mAccount->getWalletCount();c++ ){
//            //say("Wallet Currency: " + mAccount->getWallet(c)->mCurrency );
//            if ( mAccount->getWallet(c)->mCurrency == currencyBought ) {
//                destAccount = mAccount->getWallet(c)->mId;
//            }
//        }
//        req->setPath("/v2/accounts/" + destAccount + "/buys");
//        req->setBody(reqBody);
//        req->setType("buySpot"); //just for us
//        req->sendRequest();
//        cbTabLog("# Buy Requested");
//    } else {
//        say("Payment Method Not Selected...");
//        say("Select a Payment Method!");
//    }

//    //close off printout
//    cbTabLog("################## ");
//    cbTabLog(" ");
//}

//COIN BASE SELL FEES
//subtotal -> Fee
//------------

//For ETH fees are:
//$1-$10 = $0.99
//$10-$25 = $1.49
//$25-$50 = $1.99
//$50-$200 = $2.99
//$200+ = 1.49%

//LTC:
//$1-$10 = $0.99
//$10-$25 = $1.49
//$25-$50 = $1.99
//$50-$200 = $2.99
//$200+ = 1.49%

//BTC:
//$1-$10 = $0.99
//$10-$25 = $1.49
//$25-$50 = $1.99
//$50-$200 = $2.99
//$200+ = 1.49%

//void cbApiHandler::sellSpotClicked() {
//    cbTabLog("### SELL @ SPOT PRICE ### ");
//    QString sellAmount = mParentProphet->mParent->getSellSpotAmount()->text();
//    cbTabLog("# Selling " + sellAmount + " " + mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() );
//    QString spot;
//    if ( mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() == "BTC" ) {
//        spot = mParentProphet->mParent->getBtcSpotSellPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() == "LTC" ) {
//        spot = mParentProphet->mParent->getLtcSpotSellPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() == "ETH" ) {
//        spot = mParentProphet->mParent->getEthSpotSellPriceLabel()->text();
//    }
//    cbTabLog("# Spot Price $" + spot);
//    QString subTotal = QString().setNum(mParentProphet->mParent->getSellSpotTotalLabel()->text().toDouble() + mParentProphet->mParent->getSellSpotFeeLabel()->text().toDouble());
//    subTotal = trimPriceStringDecimal(subTotal);
//    cbTabLog("# Sub-Total: $" + subTotal );
//    QString fee = mParentProphet->findCoinbaseFee(subTotal);
//    fee = trimPriceStringDecimal(fee);
//    cbTabLog("# Actual Sell Fee: $" + fee );
//    QString totalSellAmountAfterFee = QString().setNum(subTotal.toDouble() - fee.toDouble());
//    totalSellAmountAfterFee = trimPriceStringDecimal(totalSellAmountAfterFee);
//    cbTabLog("# Actual Sell Total: $" + totalSellAmountAfterFee );
//    cbTabLog("# Placing Sell");
//    // Send Request
//    QComboBox *sellSpotWith = mParentProphet->mParent->getSellSpotWithPaymentMethodComboBox();
//    QString selectedPayment = sellSpotWith->currentData().toString();
//    if ( selectedPayment.length() > 0 ) {
//        QString currencySold = mParentProphet->mParent->getSellSpotTypeComboBox()->currentText();
//        QString currencyPaidTo = mParentProphet->mParent->getSellSpotWithPaymentMethodComboBox()->currentText().mid(0,3);
//        QString accId = selectedPayment;
//        QString reqBody = "{ \"total\": \"" + totalSellAmountAfterFee + "\", \"currency\": \"" + currencyPaidTo + "\", \"payment_method\": \"" + selectedPayment + "\" , \"commit\": true }";
//        //say("## rBody -> " + reqBody);
//        cbTabLog("# Sending total: " + totalSellAmountAfterFee);
//        cbTabLog("# Sending Currency: " + currencyPaidTo);
//        cbTabLog("# Sending commit: TRUE");
//        //Creating a new coinbaseApiRequest
//        cbApiRequest* req = new cbApiRequest(this);
//        req->setMethod("POST");
//        // We need the id of the account the buy will GO TO
//        // ie: BTC buy = BTC wallet
//        // find selected wallet and get id for URL
//        QString destAccount;
//        for ( int c=0;c<mAccount->getWalletCount();c++ ){
//            //say("Wallet Currency: " + mAccount->getWallet(c)->mCurrency );
//            if ( mAccount->getWallet(c)->mCurrency == currencySold ) {
//                destAccount = mAccount->getWallet(c)->mId;
//            }
//        }
//        req->setPath("/v2/accounts/" + destAccount + "/sells");
//        req->setBody(reqBody);
//        req->setType("sellSpot"); //just for us
//        req->sendRequest();
//        cbTabLog("# Sell Requested");
//    } else {
//        say("Payment Method Not Selected...");
//        say("Select a Payment Method!");
//    }

//    //close off printout
//    cbTabLog("################## ");
//    cbTabLog(" ");
//}

//void cbApiHandler::QuoteBuySpotClicked() {
//    cbTabLog("### BUY Quote @ SPOT PRICE ### ");
//    QString buyAmount = mParentProphet->mParent->getBuySpotAmount()->text();
//    cbTabLog("# Buying " + buyAmount + " " + mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() );
//    QString spot;
//    if ( mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() == "BTC" ) {
//        spot = mParentProphet->mParent->getBtcSpotBuyPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() == "LTC" ) {
//        spot = mParentProphet->mParent->getLtcSpotBuyPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getBuySpotTypeComboBox()->currentText() == "ETH" ) {
//        spot = mParentProphet->mParent->getEthSpotBuyPriceLabel()->text();
//    }
//    cbTabLog("# Spot Price $" + spot);
//    QString subTotal = QString().setNum(spot.toDouble() * buyAmount.toDouble());
//    subTotal = trimPriceStringDecimal(subTotal);
//    cbTabLog("# Sub-Total: $" + subTotal );
//    QString feePct = "1.49"; //For ACH minimum is $0.15
//    QString feeDec = QString().setNum(feePct.toDouble()*0.01);
//    QString fee = QString().setNum(subTotal.toDouble() * feeDec.toDouble());
//    cbTabLog("# Buy Fee (%): " + feePct + "%");
//    fee = trimPriceStringDecimal(fee);
//    if ( fee.toDouble() < 0.99 ) {
//        fee = "0.99";
//    }
//    cbTabLog("# Buy Fee: $" + fee );
//    QString totalBuyAmountAfterFee = QString().setNum(fee.toDouble() + subTotal.toDouble());
//    totalBuyAmountAfterFee = trimPriceStringDecimal(totalBuyAmountAfterFee);
//    cbTabLog("# Buy Total: $" + totalBuyAmountAfterFee );
//    mParentProphet->mParent->getBuySpotFeeLabel()->setText("$" + fee);
//    mParentProphet->mParent->getBuySpotTotalLabel()->setText("$" + totalBuyAmountAfterFee);
//    cbTabLog("# Confirming Quote");
//    // Send Request
//    QComboBox *buySpotWith = mParentProphet->mParent->getBuySpotWithPaymentMethodComboBox();
//    QString selectedPayment = buySpotWith->currentData().toString();
//    if ( selectedPayment.length() > 0 ) {
//        QString currencyBought = mParentProphet->mParent->getBuySpotTypeComboBox()->currentText();
//        QString currencyPaidWith = mParentProphet->mParent->getBuySpotWithPaymentMethodComboBox()->currentText().mid(0,3);
//        QString accId = selectedPayment;
//        QString reqBody = "{ \"total\": \"" + totalBuyAmountAfterFee + "\", \"currency\": \"" + currencyPaidWith + "\", \"payment_method\": \"" + selectedPayment + "\" , \"commit\": false,\"quote\": true }";
//        //say("## rBody -> " + reqBody);
//        cbTabLog("# Sending TOTAL: " + totalBuyAmountAfterFee);
//        cbTabLog("# Sending Currency: " + currencyPaidWith);
//        cbTabLog("# Sending commit: FALSE");
//        cbTabLog("# Sending quote: TRUE");
//        //cbTabLog("# Sending pm: " + selectedPayment);
//        //Creating a new coinbaseApiRequest
//        cbApiRequest* req = new cbApiRequest(this);
//        req->setMethod("POST");
//        // We need the id of the account the buy will GO TO
//        // ie: BTC buy = BTC wallet
//        // find selected wallet and get id for URL
//        QString destAccount;
//        for ( int c=0;c<mAccount->getWalletCount();c++ ){
//            //say("Wallet Currency: " + mAccount->getWallet(c)->mCurrency );
//            if ( mAccount->getWallet(c)->mCurrency == currencyBought ) {
//                destAccount = mAccount->getWallet(c)->mId;
//            }
//        }
//        req->setPath("/v2/accounts/" + destAccount + "/buys");
//        req->setBody(reqBody);
//        req->setType("buySpotQuote"); //just for us
//        req->sendRequest();
//        cbTabLog("# Quote Requested");
//    } else {
//        say("Payment Method Not Selected...");
//        say("Select a Payment Method!");
//    }

//    //close off printout
//    cbTabLog("################## ");
//    cbTabLog(" ");
//}


//void cbApiHandler::QuoteSellSpotClicked() {
//    cbTabLog("### SELL Quote @ SPOT PRICE ### ");
//    QString sellAmount = mParentProphet->mParent->getSellSpotAmount()->text();
//    cbTabLog("# Selling " + sellAmount + " " + mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() );
//    QString spot;
//    if ( mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() == "BTC" ) {
//        spot = mParentProphet->mParent->getBtcSpotSellPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() == "LTC" ) {
//        spot = mParentProphet->mParent->getLtcSpotSellPriceLabel()->text();
//    } else if ( mParentProphet->mParent->getSellSpotTypeComboBox()->currentText() == "ETH" ) {
//        spot = mParentProphet->mParent->getEthSpotSellPriceLabel()->text();
//    }
//    cbTabLog("# Spot Price $" + spot);
//    QString subTotal = QString().setNum(spot.toDouble() * sellAmount.toDouble());
//    subTotal = trimPriceStringDecimal(subTotal);
//    cbTabLog("# Sub-Total: $" + subTotal );
//    QString feePct = "Varies"; //For ACH minimum is $0.15
//    QString fee = mParentProphet->findCoinbaseFee(subTotal);
//    fee = trimPriceStringDecimal(fee);
//    cbTabLog("# Sell Fee: $" + fee );
//    QString totalSellAmountAfterFee = QString().setNum(subTotal.toDouble() - fee.toDouble());
//    totalSellAmountAfterFee = trimPriceStringDecimal(totalSellAmountAfterFee);
//    cbTabLog("# Sale Est: $" + totalSellAmountAfterFee );
//    mParentProphet->mParent->getSellSpotFeeLabel()->setText("$" + fee);
//    mParentProphet->mParent->getSellSpotTotalLabel()->setText("$" + totalSellAmountAfterFee);
//    cbTabLog("# Confirming Quote");
//    // Send Request

//    QComboBox *sellSpotWith = mParentProphet->mParent->getSellSpotWithPaymentMethodComboBox();
//    QString selectedPayment = sellSpotWith->currentData().toString();
//    if ( selectedPayment.length() > 0 ) {
//        QString currencySold = mParentProphet->mParent->getSellSpotTypeComboBox()->currentText();
//        QString currencyPaidTo = mParentProphet->mParent->getSellSpotWithPaymentMethodComboBox()->currentText().mid(0,3);
//        QString accId = selectedPayment;
//        QString reqBody = "{ \"total\": \"" + totalSellAmountAfterFee + "\", \"currency\": \"" + currencyPaidTo + "\", \"payment_method\": \"" + selectedPayment + "\" , \"commit\": false,\"quote\": true }";
//        //say("## rBody -> " + reqBody);
//        cbTabLog("# Sending TOTAL: " + totalSellAmountAfterFee);
//        cbTabLog("# Sending Currency: " + currencyPaidTo);
//        cbTabLog("# Sending commit: FALSE");
//        cbTabLog("# Sending quote: TRUE");
//        //Creating a new coinbaseApiRequest
//        cbApiRequest* req = new cbApiRequest(this);
//        req->setMethod("POST");
//        // We need the id of the account the buy will GO TO
//        // ie: BTC buy = BTC wallet
//        // find selected wallet and get id for URL
//        QString destAccount;
//        for ( int c=0;c<mAccount->getWalletCount();c++ ){
//            //say("Wallet Currency: " + mAccount->getWallet(c)->mCurrency );
//            if ( mAccount->getWallet(c)->mCurrency == currencySold ) {
//                destAccount = mAccount->getWallet(c)->mId;
//            }
//        }
//        req->setPath("/v2/accounts/" + destAccount + "/sells");
//        req->setBody(reqBody);
//        req->setType("sellSpotQuote"); //just for us
//        req->sendRequest();
//        cbTabLog("# Quote Requested");
//    } else {
//        say("Payment Method Not Selected...");
//        say("Select a Payment Method!");
//    }


//    //close off printout
//    cbTabLog("################## ");
//    cbTabLog(" ");
//}

void cbApiHandler::cbTabLog(QString logString) {
    mParentProphet->mParent->getCoinbaseTabLog()->append(logString);
    say(logString);
}

QString cbApiHandler::trimPriceStringDecimal( QString priceString){
    if ( priceString.indexOf('.') != -1 ) {
        if ( priceString.mid(priceString.indexOf('.')+1).length() < 2 ) {
            priceString.append('0');
        }
        priceString = priceString.mid(0,priceString.indexOf('.')+3);
    } else {
        priceString.append(".00");
    }
    return priceString;
}




void cbApiHandler::listAccounts() {
    // URL: https://api.coinbase.com/v2/accounts
    // method: GET
    // requestPath: /v2/accounts
    // body: EMPTY ie: ""
    //say("Fetching Account (Wallet List)");
    mParentProphet->setProphetState("FETCH");

    //Instead of manual, Creating a new coinbaseApiRequest
    cbApiRequest* req = new cbApiRequest(this);
//    mCurrentRequestList.append(req);
    req->setMethod("GET");        //list accounts is a GET
    req->setPath("/v2/accounts");  //the url path,etc..
    req->setBody("");             //no body needed (for this one)
    req->setType("listAccounts");//just for us, forgot why...
    //say("Sending Request...");
    req->sendRequest();           //sendRequest has the info/access it needs to do the rest.
    //say("Request Sent.");
}

void cbApiHandler::fetchBTCSpotPrice() {
    // https://api.coinbase.com/v2/prices/BTC-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/BTC-USD/spot");
    req->setBody("");
    req->setType("btcSpotPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchETHSpotPrice() {
    // https://api.coinbase.com/v2/prices/ETH-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/ETH-USD/spot");
    req->setBody("");
    req->setType("ethSpotPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchLTCSpotPrice() {
    // https://api.coinbase.com/v2/prices/LTC-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/LTC-USD/spot");
    req->setBody("");
    req->setType("ltcSpotPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchBTCSpotBuyPrice() {
    // https://api.coinbase.com/v2/prices/BTC-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/BTC-USD/buy");
    req->setBody("");
    req->setType("btcSpotBuyPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchETHSpotBuyPrice() {
    // https://api.coinbase.com/v2/prices/ETH-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/ETH-USD/buy");
    req->setBody("");
    req->setType("ethSpotBuyPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchLTCSpotBuyPrice() {
    // https://api.coinbase.com/v2/prices/LTC-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/LTC-USD/buy");
    req->setBody("");
    req->setType("ltcSpotBuyPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchBTCSpotSellPrice() {
    // https://api.coinbase.com/v2/prices/BTC-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/BTC-USD/sell");
    req->setBody("");
    req->setType("btcSpotSellPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchETHSpotSellPrice() {
    // https://api.coinbase.com/v2/prices/ETH-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/ETH-USD/sell");
    req->setBody("");
    req->setType("ethSpotSellPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchLTCSpotSellPrice() {
    // https://api.coinbase.com/v2/prices/LTC-USD/spot
    cbApiRequest* req = new cbApiRequest(this);
    req->setMethod("GET");
    req->setPath("/v2/prices/LTC-USD/sell");
    req->setBody("");
    req->setType("ltcSpotSellPrice");
    req->sendRequest();
    return;
}

void cbApiHandler::fetchSpotPrices() {
    fetchBTCSpotPrice();
    fetchLTCSpotPrice();
    fetchETHSpotPrice();
    QTimer::singleShot(1000,this,SLOT(fetchSpotPrices2()));

}

void cbApiHandler::fetchSpotPrices2() {
    fetchBTCSpotBuyPrice();
    fetchLTCSpotBuyPrice();
    fetchETHSpotBuyPrice();
    QTimer::singleShot(1000,this,SLOT(fetchSpotPrices3()));
}


void cbApiHandler::fetchSpotPrices3() {
    fetchBTCSpotSellPrice();
    fetchLTCSpotSellPrice();
    fetchETHSpotSellPrice();
    if ( mParentProphet->mAutoCheckSpotPrices ) {
        QTimer::singleShot(mParentProphet->mAutoCheckSpotPricesInterval,this,SLOT(fetchSpotPrices()));
    }
}

void cbApiHandler::withdrawToButtonSlot() {
     say("## WITHDRAW FIAT TO PAYMENT METHOD ##");
     QComboBox *accSelect = mParentProphet->mParent->getWithdrawFromPayMethodCombo();
     QComboBox *accTo = mParentProphet->mParent->getWithdrawToPayMethodCombo();
     QString selected = accSelect->currentData().toString();
     if ( selected.length() > 0 ) {
         QString currency = accSelect->currentText().mid(0,accSelect->currentText().indexOf(" "));
         QString pMethod = selected;
         QString withAmount = mParentProphet->mParent->getWithdrawToPayMethodAmount()->text();
         QString withTo = accTo->currentData().toString();
         QString reqBody = "{ \"amount\": \"" + withAmount + "\", \"currency\": \"" + currency + "\", \"payment_method\": \"" + withTo + "\" }";
         say("## rBody -> " + reqBody);
         //Creating a new coinbaseApiRequest
         cbApiRequest* req = new cbApiRequest(this);
         req->setMethod("POST");
         req->setPath("/v2/accounts/" + pMethod + "/withdrawals");
         req->setBody(reqBody);
         req->setType("WithdrawTo"); //just for us
         req->sendRequest();
     } else {
         say("Payment Method Not Selected...");
         say("Select a Payment Method!");
     }
}



void cbApiHandler::depositFromButtonSlot() {
     say("## DEPOSIT FIAT FROM PAYMENT METHOD ##");
     QComboBox *accSelect = mParentProphet->mParent->getDepositFromPayMethodCombo();
     QComboBox *accTo = mParentProphet->mParent->getDepositToPayMethodCombo();
     QString selected = accSelect->currentData().toString();
     if ( selected.length() > 0 ) {
         QString currency = accSelect->currentText().mid(0,accSelect->currentText().indexOf(" "));
         QString pMethod = selected;
         QString depAmount = mParentProphet->mParent->getDepositFromPayMethodAmount()->text();
         QString depTo = accTo->currentData().toString();
         QString reqBody = "{ \"amount\": \"" + depAmount + "\", \"currency\": \"" + currency + "\", \"payment_method\": \"" + pMethod + "\" }";
         say("## rBody -> " + reqBody);
         //Creating a new coinbaseApiRequest
         cbApiRequest* req = new cbApiRequest(this);
         req->setMethod("POST");
         req->setPath("/v2/accounts/" + depTo + "/deposits");
         req->setBody(reqBody);
         req->setType("DepositFrom"); //just for us
         req->sendRequest();                 //sendRequest has the info/access it needs to do the rest.
     } else {
         say("Payment Method Not Selected...");
         say("Select a Payment Method!");
     }
}


// NEXT IS AUTOTRADING.... I THINK...


