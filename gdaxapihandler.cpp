#include "gdaxapihandler.h"

gdaxApiHandler::gdaxApiHandler(bitProphet *parent) : QObject(parent), mAccount(NULL),mWalletTableWidget(NULL) {
    mPtrName = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    mParent=parent;
    mProductIds.append("BTC-USD");
    mProductIds.append("LTC-USD");
    mProductIds.append("ETH-USD");
    say("Created GDAX Api Handler.");    
    if ( mParent->getDb()->getGdaxAccountList().length() > 0 ) {
        QString defGdaxId = mParent->getDb()->getDefaultGdaxAccountId();
        if ( defGdaxId != "0" ) {
            say("Found Default GDAX Account. [ " + defGdaxId +" ]");
            mParent->mParent->getAutoRefreshGdaxBalanceEnabledCheckBox()->setEnabled(true);
            mParent->mParent->getGdaxAutoTradeEnabledCheckBox()->setEnabled(true);
            if ( !mParent->getDb()->hasTable("gdaxPriceHistory") ) {
                mParent->getDb()->createGdaxPriceHistoryTable();
                say("Created gdaxPriceHistory Table.");
            } else {
                //say("Found gdaxPriceHistory Table.");
            }
            if ( !mParent->getDb()->hasTable("gdaxAutoTraderHistory") ) {
                mParent->getDb()->createGdaxTraderHistoryTable();
                say("Created gdaxTraderHistory Table.");
            } else {
                //say("Found gdaxTraderHistory Table.");
            }

            mAccount = new gdaxAccount(this);
            //Load Default Account
            mParent->getDb()->loadGdaxAccountById(mAccount,defGdaxId);
            //Load accounts tab dropdown with accounts
            QList<QString> daList= mParent->getDb()->getGdaxAccountList();
            for ( int c=0;c<daList.length();c++) {
                mParent->mParent->getGdaxAccountComboBox()->addItem(daList.at(c));
                say("Gdax Account: " + daList.at(c));
            }
            //Check GDAX balances once
            listGdaxAccounts();
            //fetch our coinbase accounts and load them into combo boxes for transfers
            listCoinbaseAccountsAvailableToGdax();
            //do 1 price check
            //If we use websocket... we dont use this
            for(int c=0;c<mProductIds.count();c++){
                fetchGdaxPrice(mProductIds.at(c));
            }
        } else {
            say("GDAX Api Handler Has No Default Account.");
            say("Use Setup Menu to add one.");
        }
    } else {
        say("No GDAX Accounts Exist In Database.");
        say("Use Setup Menu to add one.");
    }
    say("GDAX Api Handler Loaded.");
}


gdaxApiHandler::~gdaxApiHandler() {
    if (mAccount != NULL) { delete mAccount; }
    say("GDAX Api Handler Fading...");
}

///////////
// Methods
///////////
void gdaxApiHandler::say(QString sayThis) {
    mParent->say( "[gdaxApiHandler] " + sayThis);
}

void gdaxApiHandler::listGdaxAccounts() {
    // URL: https://api.gdax.com/v2/accounts
    // method: GET
    // requestPath: /accounts
    // body: EMPTY
//    say("Fetching GDAX Accounts (GDAX Wallet List)");
    mParent->setProphetState("FETCH");

    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("GET");        //list accounts is a GET
    req->setPath("/accounts");  //the url path,etc..
    req->setBody("");             //no body needed (for this one)
    req->setType("listGdaxAccounts"); //just for us
    say("Sending Request...");
    req->sendRequest();           //sendRequest has the info/access it needs to do the rest.
    say("Request Sent.");
}

void gdaxApiHandler::listCoinbaseAccountsAvailableToGdax() {
    say("Fetching Coinbase Accounts (Available To Gdax)");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("GET");        //list accounts is a GET
    req->setPath("/coinbase-accounts");  //the url path,etc..
    req->setBody("");             //no body needed (for this one)
    req->setType("listCoinbaseAccounts"); //just for us
    say("Sending Request...");
    req->sendRequest();           //sendRequest has the info/access it needs to do the rest.
    say("Request Sent.");
}

void gdaxApiHandler::xferFromCoinbaseToGdax(QString fromAccId, QString amount,QString currency) {
    say("Xferring " + amount + " of " + currency + " From Coinbase to Gdax.");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("POST");        //list accounts is a GET
    req->setPath("/deposits/coinbase-account");  //the url path,etc..
    req->setBody("{"
                 "\"amount\": " + amount + ","
                 "\"currency\": \"" + currency + "\"," +
                 "\"coinbase_account_id\": \"" + fromAccId + "\"" +
                 "}");
    req->setType("xferFromCoinbaseToGdax");
//    say("Sending Request...");
    req->sendRequest();           //sendRequest has the info/access it needs to do the rest.
//    say("Request Sent.");
//    say("Amount: " + amount);
//    say("Currency: " + currency);
//    say("coinbase_account_id: " + fromAccId);
}

//
// POST /withdrawals/coinbase
void gdaxApiHandler::xferFromGdaxToCoinbase(QString fromAccId, QString amount,QString currency) {
    say("Xferring " + amount + " of " + currency + " From Gdax To Coinbase.");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("POST");        //list accounts is a GET
    req->setPath("/withdrawals/coinbase-account");  //the url path,etc..
    req->setBody("{"
                 "\"amount\": " + amount + ","
                 "\"currency\": \"" + currency + "\"," +
                 "\"coinbase_account_id\": \"" + fromAccId + "\"" +
                 "}");
    req->setType("xferFromGdaxToCoinbase");
    req->sendRequest();           //sendRequest has the info/access it needs to do the rest.
}

void  gdaxApiHandler::placeGdaxLimitBuy(QString prodId,QString size, QString price) {
    say("Placing Limit Buy " + size + " of " + prodId + ".");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("POST");        //list accounts is a GET
    req->setPath("/orders");  //the url path,etc..
    req->setBody("{"
                 "\"size\": " + size + ","
                 "\"price\": \"" + price + "\"," +
                 "\"product_id\": \"" + prodId + "\"," +
                 "\"side\": \"buy\"" +
                 "}");
    req->setType("placeGdaxLimitBuy");
    req->sendRequest();
}

void  gdaxApiHandler::placeGdaxAutoTraderLimitBuy(QString prodId,QString size, QString price, int autoTradeId) {
    say("Placing Limit Buy " + size + " of " + prodId + ".");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("POST");
    req->setPath("/orders");
    req->setBody("{"
                 "\"size\": " + size + ","
                 "\"price\": \"" + price + "\"," +
                 "\"product_id\": \"" + prodId + "\"," +
                 "\"side\": \"buy\"" +
                 "}");
    req->setType("placeGdaxAutoTraderLimitBuy");
    req->mAutoTradeId = QString().setNum(autoTradeId);
    req->sendRequest();
}

void  gdaxApiHandler::placeGdaxAutoTraderLimitSell(QString prodId,QString size, QString price, int autoTradeId) {
    say("Placing Limit Sell " + size + " of " + prodId + " @ $" + price);
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("POST");
    req->setPath("/orders");
    req->setBody("{"
                 "\"size\": " + size + ","
                 "\"price\": \"" + price + "\"," +
                 "\"product_id\": \"" + prodId + "\"," +
                 "\"side\": \"sell\"" +
                 "}");
    req->setType("placeGdaxAutoTraderLimitSell");
    req->mAutoTradeId = QString().setNum(autoTradeId);
    req->sendRequest();
}


void  gdaxApiHandler::fetchGdaxFillsForOrderId(QString orderId) {
    mParent->setProphetState("FETCH");
    QString autoId = mParent->getDb()->getGdaxAutoTradeIdByOrderId(orderId);
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("GET");        //list accounts is a GET
    req->setPath("/orders/" + orderId);  //the url path,etc..
    req->setBody("");
    req->setType("fetchGdaxFillsForOrderId");
    req->mAutoTradeId = autoId;
    req->sendRequest();
}

void  gdaxApiHandler::fetchGdaxSellFillsForOrderId(QString orderId) {
    mParent->setProphetState("FETCH");
    QString autoId = mParent->getDb()->getGdaxAutoTradeIdByOrderId(orderId);
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("GET");
    req->setPath("/orders/" + orderId);
    req->setBody("");
    req->setType("fetchGdaxSellFillsForOrderId");
    req->mAutoTradeId = autoId;
    req->sendRequest();
}


void  gdaxApiHandler::placeGdaxLimitSell(QString prodId,QString size, QString price) {
    say("Placing Limit Sell " + size + " of " + prodId + ".");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("POST");        //list accounts is a GET
    req->setPath("/orders");  //the url path,etc..
    req->setBody("{"
                 "\"size\": " + size + ","
                 "\"price\": \"" + price + "\"," +
                 "\"product_id\": \"" + prodId + "\"," +
                 "\"side\": \"sell\"" +
                 "}");
    req->setType("placeGdaxLimitSell");
    req->sendRequest();
}

void gdaxApiHandler::cancelAllGdaxOrders() {
    say("Cancelling All GDAX Orders.");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("DELETE");        //list accounts is a GET
    req->setPath("/orders");  //the url path,etc..
    req->setBody("");             //no body needed (for this one)
    req->setType("cancelAllGdaxOrders"); //just for us
    req->sendRequest();
}

void gdaxApiHandler::fetchGdaxPrice(QString prodId) {
//    say("GDAX priceCheck");
    mParent->setProphetState("FETCH");
    gdaxApiRequest* req = new gdaxApiRequest(this);
    req->setMethod("GET");
    req->setPath("/products/" + prodId + "/ticker");
    req->setBody("");
    req->setType("fetchGdaxPrices" + prodId);
    req->sendRequest();
    //if mAutoRefreshGdaxPrices, restart timer when response is received (not here)
}

QString gdaxApiHandler::getGdaxApiKey() {
    return mAccount->mApiKey;
}

QString gdaxApiHandler::getGdaxApiSecret() {
    return mAccount->mApiSecret;
}

QString gdaxApiHandler::getGdaxPassPhrase() {
    return mAccount->mPassPhrase;
}

///////////////////////
// RESPONSE PROCESSORS
///////////////////////

void gdaxApiHandler::processResponse( gdaxApiResponse *resp ) {
    QString type = resp->getType();
//    say("Processing Response Type: " + type);
    if (type == "listGdaxAccounts" ) {
        say("Got Gdax Wallets...");
        listAccountProcessResponse(resp);
        if ( mParent->mAutoRefreshGdaxAccount ) {
            int timer= mParent->mAutoRefreshGdaxAccountInterval;
            QTimer::singleShot(timer, this, SLOT(listGdaxAccountsSlot()));
        }
    } else if (type == "listCoinbaseAccounts" ) {
        listCoinbaseAccountsProcessResponse(resp);
    } else if ( type == "fetchGdaxPricesBTC-USD") {
        fetchGdaxPriceProcessResponse(resp,"BTC-USD");
    } else if ( type == "fetchGdaxPricesLTC-USD") {
        fetchGdaxPriceProcessResponse(resp,"LTC-USD");
    } else if ( type == "fetchGdaxPricesETH-USD") {
        fetchGdaxPriceProcessResponse(resp,"ETH-USD");
    } else if ( type== "placeGdaxAutoTraderLimitBuy" ) {
        placeGdaxAutoTraderLimitBuyProcessResponse(resp);
    } else if ( type== "placeGdaxAutoTraderLimitSell" ) {
        placeGdaxAutoTraderLimitSellProcessResponse(resp);
    } else if ( type == "fetchGdaxFillsForOrderId" ) {
        fetchGdaxFillsForOrderIdProcessResponse(resp);
    } else if ( type == "fetchGdaxSellFillsForOrderId") {
        fetchGdaxSellFillsForOrderIdProcessResponse(resp);
    } else if ( type == "xferFromGdaxToCoinbase") {
        xferFromGdaxToCoinbaseProcessResponse(resp);
    } else {
        say("Unknown Response Type: " + type);
    }
    mParent->mParent->getGdaxStatProcessedLabel()->setText(QString().setNum(mParent->mParent->getGdaxStatProcessedLabel()->text().toInt() + 1));
//    say("Done Processing Response Type: " + type);
}

void gdaxApiHandler::listAccountProcessResponse(gdaxApiResponse *resp ) {
    if ( mAccount->getWalletCount() > 0 ) {
        for ( int a=0;a<mAccount->getWalletCount();a++) {
            mAccount->getWallet(a)->deleteLater();
        }
        mAccount->clearWallets();
    }
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonArray arr = *resp->getResponseArray();
//    say("Items: " + QString().setNum(arr.count()) );
    QComboBox *xferFromTarget = mParent->mParent->getXferFromCbWalletTargetComboBox();
    QComboBox *xferToSource = mParent->mParent->getXferToCbWalletSourceComboBox();
    QComboBox *placeLimitBuySource = mParent->mParent->getPlaceGdaxLimitBuySourceComboBox();
    QComboBox *placeLimitSellTarget = mParent->mParent->getPlaceGdaxLimitSellTargetComboBox();
    bool addLimitSellUsdWallets = true;
    bool addXfers = true;
    if ( xferFromTarget->count() > 0) { addXfers = false; }
    if ( placeLimitBuySource ->count() > 0) { addLimitSellUsdWallets = false; }
    for(int w=0;w<arr.count();w++) {
        int newWalletId = mAccount->addWallet();
        mAccount->getWallet(newWalletId)->mId = arr.at(w).toObject()["id"].toString(); //store entire string
        mAccount->getWallet(newWalletId)->mCurrency = arr.at(w).toObject()["currency"].toString();
        mAccount->getWallet(newWalletId)->mBalance = arr.at(w).toObject()["balance"].toString();
        mAccount->getWallet(newWalletId)->mAvailable = arr.at(w).toObject()["available"].toString();
        mAccount->getWallet(newWalletId)->mHold = arr.at(w).toObject()["hold"].toString();
        mAccount->getWallet(newWalletId)->mProfileId = arr.at(w).toObject()["profile_id"].toString();
        if ( addXfers ) {
            xferFromTarget->addItem(arr.at(w).toObject()["currency"].toString() + " [" + arr.at(w).toObject()["id"].toString().mid(0,8) + "]",QVariant(arr.at(w).toObject()["id"].toString()));
            xferToSource->addItem(arr.at(w).toObject()["currency"].toString() + " [" + arr.at(w).toObject()["id"].toString().mid(0,8) + "]",QVariant(arr.at(w).toObject()["id"].toString()));
        }
        if ( addLimitSellUsdWallets ) {
            if ( arr.at(w).toObject()["currency"].toString() == "USD" ) {
                placeLimitBuySource->addItem(arr.at(w).toObject()["currency"].toString() + " [" + arr.at(w).toObject()["id"].toString().mid(0,8) + "]", QVariant(arr.at(w).toObject()["id"].toString()) );
                placeLimitSellTarget->addItem(arr.at(w).toObject()["currency"].toString() + " [" + arr.at(w).toObject()["id"].toString().mid(0,8) + "]", QVariant(arr.at(w).toObject()["id"].toString()) );
            }
        }
    }
    if ( mWalletTableWidget != NULL ) {
        mWalletTableWidget->deleteLater();
    }
    mWalletTableWidget = new gdaxWalletTable(mAccount,this);
//    say("Processed " + QString().setNum(mAccount->getWalletCount()) + " Wallets.");
}

void gdaxApiHandler::listCoinbaseAccountsProcessResponse(gdaxApiResponse *resp ) {
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonArray arr = *resp->getResponseArray();
//    say("Items: " + QString().setNum(arr.count()) );
    if ( arr.count() > 0 ) {
        mParent->mParent->getXferFromCbWalletButton()->setEnabled(true);
        mParent->mParent->getXferToCbWalletButton()->setEnabled(true);
    }
    for(int w=0;w<arr.count();w++) {
//        say("id: " + arr.at(w).toObject()["id"].toString().mid(0,8) );
        QComboBox *xferFrom = mParent->mParent->getXferFromCbWalletComboBox();
        QComboBox *xferTo = mParent->mParent->getXferToCbWalletComboBox();
        xferFrom->addItem(arr.at(w).toObject()["currency"].toString() + " [" + arr.at(w).toObject()["id"].toString().mid(0,8) + "]",QVariant(arr.at(w).toObject()["id"].toString()));
        xferTo->addItem(arr.at(w).toObject()["currency"].toString() + " [" + arr.at(w).toObject()["id"].toString().mid(0,8) + "]",QVariant(arr.at(w).toObject()["id"].toString()));
    }
}

void gdaxApiHandler::fetchGdaxPriceProcessResponse(gdaxApiResponse *resp,QString productId) {
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonObject obj = *resp->getResponseContent();
    //price,bid,ask
//    say("<["+ productId + "]>");
    //pretty it up
    QString p,a,b;
    if ( obj["price"].toString().indexOf(".",0) != -1 ) {
        p = obj["price"].toString().mid(obj["price"].toString().indexOf(".",0)+1,2) ;
        if ( p.length() == 1 ) { p+="0"; }
        p = obj["price"].toString().mid(0,obj["price"].toString().indexOf(".",0)) + "." + p;
    } else {
        p = obj["price"].toString() + ".00";
    }

    if ( obj["ask"].toString().indexOf(".",0) != -1 ) {
        a = obj["ask"].toString().mid(obj["ask"].toString().indexOf(".",0)+1,2);
        if ( a.length() == 1 ) { a+="0"; }
        a = obj["ask"].toString().mid(0,obj["ask"].toString().indexOf(".",0)) + "." + a;
    } else {
       a = obj["ask"].toString() + ".00";
    }

    if ( obj["bid"].toString().indexOf(".",0) != -1 ) {
        b = obj["bid"].toString().mid(obj["bid"].toString().indexOf(".",0)+1,2);
        if ( b.length() == 1 ) { b+="0"; }
        b = obj["bid"].toString().mid(0,obj["bid"].toString().indexOf(".",0)) + "." + b;
    } else {
        b = obj["bid"].toString() + ".00";
    }

//    say("Last: " + p );
//    say("Ask: " + a );
//    say("Bid: " + b );
    //update price label text
    if (productId == "BTC-USD" ){
        mParent->mParent->getGdaxBtcPriceLabel()->setText( p );
        mParent->mParent->getGdaxBtcAskLabel()->setText( a );
        mParent->mParent->getGdaxBtcBidLabel()->setText( b );
        if ( mParent->mAutoCheckGDAXPrices ) {
            int timer= mParent->mAutoCheckGDAXPricesInterval;
            QTimer::singleShot(timer, this, SLOT(fetchGdaxPriceSlotBtc()));
        }
        mParent->getDb()->addToGdaxPriceHistory("BTC",p,a,b);
    } else if (productId == "ETH-USD" ){
        mParent->mParent->getGdaxEthPriceLabel()->setText( p );
        mParent->mParent->getGdaxEthAskLabel()->setText( a );
        mParent->mParent->getGdaxEthBidLabel()->setText( b );
        if ( mParent->mAutoCheckGDAXPrices ) {
            int timer= mParent->mAutoCheckGDAXPricesInterval;
            QTimer::singleShot(timer, this, SLOT(fetchGdaxPriceSlotEth()));
        }
        mParent->getDb()->addToGdaxPriceHistory("ETH",p,a,b);
    } else if (productId == "LTC-USD" ){
        mParent->mParent->getGdaxLtcPriceLabel()->setText( p );
        mParent->mParent->getGdaxLtcAskLabel()->setText( a );
        mParent->mParent->getGdaxLtcBidLabel()->setText( b );
        if ( mParent->mAutoCheckGDAXPrices ) {
            int timer= mParent->mAutoCheckGDAXPricesInterval;
            QTimer::singleShot(timer, this, SLOT(fetchGdaxPriceSlotLtc()));
        }
        mParent->getDb()->addToGdaxPriceHistory("LTC",p,a,b);
    }
}

void gdaxApiHandler::placeGdaxAutoTraderLimitBuyProcessResponse(gdaxApiResponse *resp) {
    QString tradeId(resp->mAutoTradeId);
    //if successful, update gdaxTraderHistory entry
    //if failed, remove gdaxTraderHistor entry
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonObject obj = *resp->getResponseContent();
    QString orderId = obj["id"].toString();
    if ( orderId.length() > 0 ) {
        say("# Gdax Auto Buy orderId - "+orderId+" was placed!");
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","orderId",orderId);
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","status","placed2"); //placed is sent, placed2 is confirmed successful placed.
    }

}

void gdaxApiHandler::placeGdaxAutoTraderLimitSellProcessResponse(gdaxApiResponse *resp) {
    QString tradeId(resp->mAutoTradeId);
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonObject obj = *resp->getResponseContent();
    QString orderId = obj["id"].toString();
    if ( orderId.length() > 0 ) {
        say("# Gdax Auto Sell orderId - "+orderId+" was placed!");
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","orderId",orderId);
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","status","posted2"); //posted is sell sent, posted2 is confirmed successful placed sell.
    }
}


void gdaxApiHandler::fetchGdaxSellFillsForOrderIdProcessResponse(gdaxApiResponse *resp) {
    QString tradeId(resp->mAutoTradeId);
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonObject obj = *resp->getResponseContent();
    bool isSettled = obj["settled"].toBool();
    if ( isSettled ) {
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","status","SOLD");
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","type","SELL");
        QString fees = obj["fill_fees"].toString();
        QString feeMod = "0.0";
        if ( fees.toDouble() > 0.0 ) { feeMod = fees; }
        QString sellPrice = mParent->getDb()->getGdaxAutoTradeHistoryValueById(tradeId,"sellTarget");
        QString sellTotal = mParent->getDb()->getGdaxAutoTradeHistoryValueById(tradeId,"sellTotal");
        sellTotal = QString().setNum( sellTotal.toDouble() - feeMod.toDouble() );
        if ( sellTotal.indexOf(".",0) != -1 ) {
            //if it has a demical place
            QString pre=sellTotal.mid(0,sellTotal.indexOf(".",0));
            QString post= sellTotal.mid(sellTotal.indexOf(".",0)+1,2);
            sellTotal = pre + "." + post;
        }
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","sellTotal",sellTotal);
        //find the profit and send it back to coinbase in USD
        QString buyTotal = mParent->getDb()->getGdaxAutoTradeHistoryValueById(tradeId,"buyTotal");
        QString finalProfit = QString().setNum(sellTotal.toDouble() - buyTotal.toDouble());
        say("# Gdax Auto Sell Final Profit -> $"+finalProfit );
        QComboBox *scanCurs = mParent->mParent->getXferToCbWalletComboBox();
        QString toAccId;
        for (int c=0;c<scanCurs->count();c++) {
            if ( scanCurs->itemText(c).mid(0,3) == "USD" ) {
                toAccId = scanCurs->itemData(c).toString();
            } else {
                say("ScanCurs:" + scanCurs->itemText(c).mid(0,3) );
            }
        }
        xferFromGdaxToCoinbase( toAccId,finalProfit,"USD");
    }
}

void gdaxApiHandler::fetchGdaxFillsForOrderIdProcessResponse(gdaxApiResponse *resp) {
    QString tradeId(resp->mAutoTradeId);
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonObject obj = *resp->getResponseContent();
    bool isSettled = obj["settled"].toBool();
    if ( isSettled ) {
        //If its not settled, ignore it (for now)
        // if it is settled, change it to a sell and post it for sellTarget
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","status","posted");
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","type","SELL");
        QString fees = obj["fill_fees"].toString();
        QString feeMod = "0.0";
        if ( fees.toDouble() > 0.0 ) { feeMod = fees; }
        QString amount = mParent->getDb()->getGdaxAutoTradeHistoryValueById(tradeId,"amount");
        QString sellPrice = mParent->getDb()->getGdaxAutoTradeHistoryValueById(tradeId,"sellTarget");
        sellPrice = QString().setNum( sellPrice.toDouble() + feeMod.toDouble() );
        if ( sellPrice.indexOf(".",0) != -1 ) {
            //if it has a demical place
            QString pre=sellPrice.mid(0,sellPrice.indexOf(".",0));
            QString post= sellPrice.mid(sellPrice.indexOf(".",0)+1,2);
            sellPrice = pre + "." + post;
        }
        mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","sellTarget",sellPrice);
        QString coin = mParent->getDb()->getGdaxAutoTradeHistoryValueById(tradeId,"coin");
        //Now place it for sale @ sellPrice
        placeGdaxAutoTraderLimitSell(coin+"-USD",amount,sellPrice,tradeId.toInt());
        //HOLY FUCKSTICKS WE ARE DONE, LET LOOSE THE MONEY MACHINE
        // its not really done, but I can let it run by itself now.
        // time for a test run..... (gimme MONEY!)
        // it works. needs tweaks... it works... made a buck already. trading on 5.2 LTC @ 50 bux each today
        // fluctuations between $50-$51 are pulling 10-20 cents each, with varying frequency.
        // this is good, can be better.
    }
}

void gdaxApiHandler::xferFromGdaxToCoinbaseProcessResponse(gdaxApiResponse *resp) {
    QString type = resp->getType();
//    say("Processing Response >>> " + type);
    QJsonObject obj = *resp->getResponseContent();
    QString amount = obj["amount"].toString();
    QString currency = obj["currency"].toString();
    say("# Xfer Amount: "+amount+" ("+currency+")");
}

void gdaxApiHandler::process404(gdaxApiResponse *resp) {
    QString type = resp->getType();
    if ( type == "fetchGdaxSellFillsForOrderId") {
        QString tradeId(resp->mAutoTradeId);
        QJsonObject obj = *resp->getResponseContent();
        if(obj["message"] == "NotFound") {
            //we checked on an order and got notfound, means we cancelled it.
            if ( tradeId.toInt() > 0 ) {
                mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","status","CANCELLED");
                say("# Sell Order Number " + tradeId + " was cancelled, updating orderDb (NotFound)");
            }
        }
    } else if (type == "fetchGdaxFillsForOrderId") {
        QString tradeId(resp->mAutoTradeId);
        QJsonObject obj = *resp->getResponseContent();
        if(obj["message"] == "NotFound") {
            //we checked on an order and got notfound, means we cancelled it.
            if ( tradeId.toInt() > 0 ) {
                mParent->getDb()->updateRowById(tradeId,"gdaxAutoTraderHistory","status","CANCELLED");
                say("# Buy Order Number " + tradeId + " was cancelled, updating orderDb (NotFound)");
            }
        }
    } else {
        say("Process404 doesnt know about >>>> " + type);
    }
}

/////////
// Slots
/////////
void gdaxApiHandler::listGdaxAccountsSlot() {
    listGdaxAccounts();
}

void gdaxApiHandler::fetchGdaxPriceSlotBtc() {
    fetchGdaxPrice("BTC-USD");
}

void gdaxApiHandler::fetchGdaxPriceSlotLtc() {
    fetchGdaxPrice("LTC-USD");
}

void gdaxApiHandler::fetchGdaxPriceSlotEth() {
    fetchGdaxPrice("ETH-USD");
}
