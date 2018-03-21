#include "cbautospottrader.h"

cbAutoSpotTrader::cbAutoSpotTrader(bitProphet *parent) : QObject(parent), mParent(parent) {
    mTradeTypes.append("LTC");
    mTradeTypes.append("ETH");
    mTradeTypes.append("BTC");
    mBuyTypes.append("LTC");
    //mBuyTypes.append("ETH");
//    mBTCLog = mParent->mParent->getAutoTraderBTCLog();
//    mLTCLog = mParent->mParent->getAutoTraderLTCLog();
//    mETHLog = mParent->mParent->getAutoTraderETHLog();
    mBTCLog->document()->setMaximumBlockCount(256);
    mLTCLog->document()->setMaximumBlockCount(256);
    mETHLog->document()->setMaximumBlockCount(256);
    mLastSellPriceBtc = "0.00";
    mLastSellPriceLtc = "0.00";
    mLastSellPriceEth = "0.00";
    say("# AutoSpot Trader Exists #");
}

cbAutoSpotTrader::~cbAutoSpotTrader() {
    say("# AutoSpot Trader Fades #");
}

///////////
// METHODS
//////////

void cbAutoSpotTrader::say(QString sayIt,QString coinLog) {
    //std::cout<<"[cbAutoSpotTrader] "<<sayIt.toStdString().c_str()<<std::endl;
    if ( coinLog == "BTC" ) {
        mBTCLog->append(sayIt);
    }else if ( coinLog == "LTC" ) {
        mLTCLog->append(sayIt);
    } else if ( coinLog == "ETH" ) {
        mETHLog->append(sayIt);
    } else {
        mBTCLog->append(sayIt);
        mLTCLog->append(sayIt);
        mETHLog->append(sayIt);
    }
}


/////////
// SLOTS
////////

void cbAutoSpotTrader::autoTradeCheck() {
    if ( mParent->mAutoSpotTrade == false ) { return; }
    int hourRange = 2;
    QString USDBalance("0.00");
    for ( int a=0;a<mParent->getHandlerAccount()->getWalletCount();a++ ) {
        if ( mParent->getHandlerAccount()->getWallet(a)->mCurrency == "USD" && mParent->getHandlerAccount()->getWallet(a)->mType == "fiat") {
            USDBalance =  mParent->getHandlerAccount()->getWallet(a)->mAmount;
        }
    }
    for (int c=0;c<mBuyTypes.length();c++) {
        QString currCoin = mBuyTypes.at(c);
        if ( USDBalance.toDouble() < 1.00) {
            say("# Not enough money, fail.",currCoin);
            break;
        }
        QString howMuchToSpend("0.00");
        if ( USDBalance.toDouble() * 0.20 > 200.01 ) {
            howMuchToSpend = QString().setNum(USDBalance.toDouble() * 0.20);
        } else if ( USDBalance.toDouble() * 0.3 > 150.01 ) {
            howMuchToSpend = QString().setNum(USDBalance.toDouble() * 0.30);
        } else if ( USDBalance.toDouble() * 0.5 > 100.01 ) { //put at least 100 bucks in you cheap bastard
            howMuchToSpend = QString().setNum(USDBalance.toDouble() * 0.50);
        } else if ( USDBalance.toDouble() > 80.01 ) {
            howMuchToSpend = QString().setNum(USDBalance.toDouble());
        } else if ( USDBalance.toDouble() > 50.01 ) {
            howMuchToSpend = QString().setNum(USDBalance.toDouble());
        } else {
            say("# Not enough money, fail.",currCoin);
            break;
        }
        QString currPrice;
        say("#################",currCoin);
        say("# Analyzing Price History",currCoin);
        say("# Coin: " + currCoin,currCoin);
        QList<QString> lastPriceRange;
        mParent->getDb()->getSpotPriceHistoryLast(currCoin,600*hourRange,&lastPriceRange); //600 @ 5.5s is 1hour
        if ( lastPriceRange.length() > 0 ) {
            currPrice = lastPriceRange.last();
        }
        say("# " + QString().setNum(lastPriceRange.count()) + " Prices",currCoin);
        say("# Current: $" + currPrice,currCoin);
        bool priceRoof = false;
        if ( currCoin == "BTC" ) {
            if ( currPrice.toDouble() >= mLastSellPriceBtc.toDouble() && mLastSellPriceBtc != "0.00" ) {
                priceRoof = true; //STOP BUYING until price drops below the last price we sold on
            }
        } else if ( currCoin == "LTC" ) {
            if ( currPrice.toDouble() >= mLastSellPriceLtc.toDouble() && mLastSellPriceLtc != "0.00" ) {
                priceRoof = true; //STOP BUYING until price drops below the last price we sold on
            }
        } else if ( currCoin == "ETH" ) {
            if ( currPrice.toDouble() >= mLastSellPriceEth.toDouble() && mLastSellPriceEth != "0.00" ) {
                priceRoof = true; //STOP BUYING until price drops below the last price we sold on
            }
        }
        say("#################",currCoin);
        //check for high price (ranged)
        QString high = findHighestPrice(lastPriceRange);
        say("# High: $" + high,currCoin);
        //check for low price (ranged)
        QString low = findLowestPrice(lastPriceRange);
        say("# Low : $" + low,currCoin);
        // Check the gap!
        QString gap = findPriceGap(lastPriceRange);
        say("# Gap : $" + gap,currCoin);

        // 1/10th of the gap is how far from the bottom and top we must be.
        // ie: if gap is 20(lo0hi20), 2 is 1/10th which means valid range is 2 to 18, 1 and
        QString tenthGap = QString().setNum( gap.toDouble() * 0.10 );
        if ( tenthGap.toDouble() < 0.99 ) { //clamp tenthgap to 0.99 OR 45% of gap (could be less than a buck difference)
            //Theres no avoiding huge fees on spot trading, autoSpot doesnt quote buys, he just grabs em
            if ( tenthGap.toDouble() < (gap.toDouble() * 0.45) ) {
                tenthGap = QString().setNum(gap.toDouble() * 0.45);
            }
        }
        say("# 10%ofGap : $" + tenthGap,currCoin);
        bool hiPass;
        bool loPass;
        // TODO add check for upward run (is prior price slightly lower?)
        //check range from top
        if ( high.toDouble() - tenthGap.toDouble() > lastPriceRange.last().toDouble() ) {
            hiPass = true;
        } else { hiPass = false; }
        //check range from bottom
        if ( low.toDouble() + tenthGap.toDouble() < lastPriceRange.last().toDouble() ) {
            loPass = true;
        } else { loPass = false; }
        //go or no
        if ( loPass && hiPass && (!priceRoof) ) {
            say("# AutoSpot buying " + currCoin,currCoin);
            // Determine amount
            say("# Balance: " + USDBalance,currCoin);
            say("# Spend: " + howMuchToSpend,currCoin);
            mParent->getHandler()->buyAutoSpot(howMuchToSpend,currCoin);
            //mParent->getDb()->insertAutoSpotTrade(currCoin,"BUY",howMuchToSpend,"0.00","UNK","AUTOBUY"); //do this on the response
            USDBalance = QString().setNum( USDBalance.toDouble() - howMuchToSpend.toDouble() ); //even if the buy fails, reduce the current cycles working amount, will reset next check
        } else {
            say("# AutoSpot passes on " + currCoin,currCoin);
            if ( priceRoof ) {
                say("# Price Roof Passed",currCoin);
                say("# Waiting for Price drop",currCoin);
            }
        }
    }
    //Next check our autoBuys,
        //quote sale price,
            //deduce profit,
            //compare to minProfit
                //sell or hodl
    for (int c=0;c<mTradeTypes.length();c++) {
        QString currCoin = mTradeTypes.at(c);
//        checkAutoBuysForProfit(currCoin);
    }
    //Finally, restart timer
    if ( mParent->mAutoSpotTrade ) {
        QTimer::singleShot(mParent->mAutoSpotTradeInterval,this,SLOT(autoTradeCheck()));
    }
}

//void cbAutoSpotTrader::checkAutoBuysForProfit (QString coin) {
//    say("#################",coin);
//    say("# Check Sales "+coin,coin);
//    QList<QString> forSale;
//    QList<QString> forSaleBoughtForPrices;
//    QList<QString> forSaleCoinAmounts;
//    mParent->getDb()->getAutoSpotBuysForSaleList(&forSale,&forSaleBoughtForPrices,&forSaleCoinAmounts,coin);
//    //say("# No. For Sale: "+ QString().setNum(forSale.count()),coin);

//    for ( int z=0;z<forSale.count();z++ ) {
//        //say("# ID: "+forSale.at(z),coin);
//        //say("# Qty: "+forSaleCoinAmounts.at(z),coin);
//        //say("# Cost: "+forSaleBoughtForPrices.at(z),coin);
//        // Figure out sell price for profits
//        QString curSellPrice;
//        if ( coin == "BTC" ) {
//            curSellPrice = mParent->mParent->getBtcSpotSellPriceLabel()->text();
//        } else if ( coin == "LTC" ) {
//            curSellPrice = mParent->mParent->getLtcSpotSellPriceLabel()->text();
//        } else if ( coin == "ETH" ) {
//            curSellPrice = mParent->mParent->getEthSpotSellPriceLabel()->text();
//        }
//        QString ifSoldNow = QString().setNum(curSellPrice.toDouble() * forSaleCoinAmounts.at(z).toDouble());
//        QString sellFee = mParent->findCoinbaseFee(ifSoldNow);

//        QString ifSoldNowAfterFee = QString().setNum( ifSoldNow.toDouble() - sellFee.toDouble() );
//        //say("# Est: "+ ifSoldNowAfterFee,coin);
//        QString profitUSD = QString().setNum( ifSoldNowAfterFee.toDouble() - forSaleBoughtForPrices.at(z).toDouble() );
//        if ( forSaleBoughtForPrices.at(z).toDouble() >= ifSoldNowAfterFee.toDouble() ) {
//            say("# Est Fee: " + sellFee,coin);
//            say("# NP :" + mParent->getHandler()->trimPriceStringDecimal(profitUSD) ,coin);
//        } else {
//            say("# Est Fee: " + sellFee,coin);
//            say("# Est Profit: $" + mParent->getHandler()->trimPriceStringDecimal(profitUSD) ,coin);
//            if ( profitUSD.toDouble() > 1.00) {
//                say("# SELLNOW id: "+forSale.at(z),coin);
//                mParent->sellAutoBuyId(forSale.at(z),coin,ifSoldNowAfterFee);
//            }
//        }
//    }
//    say("#################",coin);
//    say("",coin);
//}

QString cbAutoSpotTrader::findLowestPrice(QList<QString> hayStack) {
    QString lowest = hayStack.at(0);
    for(int c=0;c<hayStack.length();c++) {
        if ( hayStack.at(c).toDouble() < lowest.toDouble() ) {
            lowest = hayStack.at(c);
        }
    }
    return lowest;
}

QString cbAutoSpotTrader::findHighestPrice(QList<QString> hayStack) {
    QString highest = hayStack.at(0);
    for(int c=0;c<hayStack.length();c++) {
        if ( hayStack.at(c).toDouble() > highest.toDouble() ) {
            highest = hayStack.at(c);
        }
    }
    return highest;
}

QString cbAutoSpotTrader::findPriceGap(QList<QString> hayStack) {
    QString gap;
    QString highest = findHighestPrice(hayStack);
    QString lowest = findLowestPrice(hayStack);
    gap = QString().setNum( highest.toDouble() - lowest.toDouble() );
    return gap;
}


//As you can see, its really hard math.... :P
//No it isnt.
