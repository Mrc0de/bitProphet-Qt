#include "bpdatabase.h"

bpDatabase::bpDatabase(QObject *parent) : QObject(parent) {    
    mPtrName = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    //say("[bpDatabase] Starting...");
}

bpDatabase::~bpDatabase() {
    //say("[bpDatabase] Dying...");
}


///////////
// Methods
///////////

void bpDatabase::createDatabase() {
   // I know it looks odd...
   {    //trust me.
       QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
       Db.setDatabaseName("bitProphet.dat");
       if (!Db.open()) {
          say("Error: connecting to database failed!");
       } else {
          say("Database: connection ok.");
       }
       Db.close();
   } //Db is gone now.
   QSqlDatabase::removeDatabase("bitProphet.dat"); //so this actually works (no warnings, no leaks)
   QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

bool bpDatabase::hasAccountsTable() {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select COUNT(*) from sqlite_master where type='table' and name='accounts'");
            if (query.exec()) {
               if (query.next()) {
                  int count = query.record().indexOf("COUNT(*)");
                  QString result = query.value(count).toString();
                  if ( result.toInt() ) {
                      retVal = true;
                  }
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

bool bpDatabase::hasTable(QString tableName) {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select COUNT(*) from sqlite_master where type='table' and name='" + tableName + "'");
            if (query.exec()) {
               if (query.next()) {
                  int count = query.record().indexOf("COUNT(*)");
                  QString result = query.value(count).toString();
                  if ( result.toInt() ) {
                      retVal = true;
                  }
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

QList<QString> bpDatabase::getAccountList() {
    QList<QString> accList;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from accounts WHERE exchange='coinbase'");
            if (query.exec()) {
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  QString idResult = query.value(idVal).toString();
                  int nameVal = query.record().indexOf("name");
                  QString nameResult = query.value(nameVal).toString();
                  accList.append(idResult+" - "+nameResult);
                  say("id: " + idResult + " name: " + nameResult);
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return accList;
}

QList<QString> bpDatabase::getGdaxAccountList() {
    QList<QString> accList;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from gdaxAccounts WHERE exchange='GDAX'");
            if (query.exec()) {
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  QString idResult = query.value(idVal).toString();
                  int nameVal = query.record().indexOf("name");
                  QString nameResult = query.value(nameVal).toString();
                  accList.append(idResult+" - "+nameResult);
                  say("id: " + idResult + " name: " + nameResult);
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return accList;
}

void bpDatabase::getSpotPriceHistoryLast(QString coin,int maxResults, QList<QString> *priceList) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from cbSpotPriceHistory WHERE coin='"+ coin +"' AND id in (select id from cbSpotPriceHistory WHERE coin='"+ coin +"' ORDER BY ts DESC LIMIT "+QString().setNum(maxResults)+") ORDER BY ts ASC "); //First(Past) to Last(current)
            if (query.exec()) {
                int y=0;
               while (query.next()) {
                  int idVal = query.record().indexOf("price");
                  priceList->append(query.value(idVal).toString());
                  y++;
               }
            } else {
                say("No Coin Prices In History For "+coin );
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::getGdaxPriceHistoryLast(QString coin,int maxResults, QList<QString> *priceList,QList<QString> *askList,QList<QString> *bidList) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from gdaxPriceHistory WHERE coin='"+ coin +"' AND id in (select id from gdaxPriceHistory WHERE coin='"+ coin +"' ORDER BY ts DESC LIMIT "+QString().setNum(maxResults)+") ORDER BY ts ASC "); //First(Past) to Last(current)
            if (query.exec()) {
                int y=0;
               while (query.next()) {
                  int pVal = query.record().indexOf("price");
                  priceList->append(query.value(pVal).toString());
                  int aVal = query.record().indexOf("ask");
                  askList->append(query.value(aVal).toString());
                  int bVal = query.record().indexOf("bid");
                  bidList->append(query.value(bVal).toString());
                  y++;
               }
            } else {
                say("No Coin Prices In History For "+coin);
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::getGdaxPriceHistoryFrom(QString coin,QString dateTimeFrom, int maxResults, QList<QString> *priceList,QList<QString> *askList,QList<QString> *bidList,QList<QString> *tsList) {
    // select ts from gdaxPriceHistory where ts > "2018-03-22 00:00:00" limit 3;
    //2018-03-22 02:26:57
    //2018-03-22 02:26:57
    //2018-03-22 02:26:57
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from gdaxPriceHistory WHERE coin='"+ coin +"' AND id in (select * from gdaxPriceHistory WHERE coin='"+ coin +"' AND ts >= '"+dateTimeFrom+"' ORDER BY ts DESC LIMIT "+QString().setNum(maxResults)+") ORDER BY ts ASC "); //First(Past) to Last(current)
            if (query.exec()) {
                int y=0;
               while (query.next()) {
                  int pVal = query.record().indexOf("price");
                  priceList->append(query.value(pVal).toString());
                  int aVal = query.record().indexOf("ask");
                  askList->append(query.value(aVal).toString());
                  int bVal = query.record().indexOf("bid");
                  bidList->append(query.value(bVal).toString());
                  int tVal = query.record().indexOf("ts");
                  tsList->append(query.value(tVal).toString());
                  y++;
               }
            } else {
                say("No Coin Prices In History For "+coin);
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");

}


void bpDatabase::getBtcSpotPriceHistoryLast(int howManyMax,bpSplineChart *chart) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from cbSpotPriceHistory WHERE coin='BTC' AND id in (select id from cbSpotPriceHistory WHERE coin='BTC' ORDER BY ts DESC LIMIT "+QString().setNum(howManyMax)+") ORDER BY ts ASC "); //spin it around

            if (query.exec()) {
                int y=0;
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  QString idResult = query.value(idVal).toString();
                  int cVal = query.record().indexOf("coin");
                  QString cResult = query.value(cVal).toString();
                  int pVal = query.record().indexOf("price");
                  QString pResult = query.value(pVal).toString();
                  int tVal = query.record().indexOf("ts");
                  QString tResult = query.value(tVal).toString();
                  //say("id: " + idResult + " coin: " + cResult + " price: " + pResult + " time: " + tResult);
                  chart->mSeries->append(y,pResult.toFloat());
                  y++;
               }
            } else {
                say("No Coin Prices In History For BTC");
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::getLtcSpotPriceHistoryLast(int howManyMax,bpSplineChart *chart) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from cbSpotPriceHistory WHERE coin='LTC' AND id in (select id from cbSpotPriceHistory WHERE coin='LTC' ORDER BY ts DESC LIMIT "+QString().setNum(howManyMax)+") ORDER BY ts ASC "); //spin it around

            if (query.exec()) {
                int y=0;
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  QString idResult = query.value(idVal).toString();
                  int cVal = query.record().indexOf("coin");
                  QString cResult = query.value(cVal).toString();
                  int pVal = query.record().indexOf("price");
                  QString pResult = query.value(pVal).toString();
                  int tVal = query.record().indexOf("ts");
                  QString tResult = query.value(tVal).toString();
                  //say("id: " + idResult + " coin: " + cResult + " price: " + pResult + " time: " + tResult);
                  chart->mSeries->append(y,pResult.toFloat());
                  y++;
               }
            } else {
                say("No Coin Prices In History For LTC");
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::getEthSpotPriceHistoryLast(int howManyMax,bpSplineChart *chart) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from cbSpotPriceHistory WHERE coin='ETH' AND id in (select id from cbSpotPriceHistory WHERE coin='ETH' ORDER BY ts DESC LIMIT "+QString().setNum(howManyMax)+") ORDER BY ts ASC "); //spin it around

            if (query.exec()) {
                int y=0;
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  QString idResult = query.value(idVal).toString();
                  int cVal = query.record().indexOf("coin");
                  QString cResult = query.value(cVal).toString();
                  int pVal = query.record().indexOf("price");
                  QString pResult = query.value(pVal).toString();
                  int tVal = query.record().indexOf("ts");
                  QString tResult = query.value(tVal).toString();
                  //say("id: " + idResult + " coin: " + cResult + " price: " + pResult + " time: " + tResult);
                  chart->mSeries->append(y,pResult.toFloat());
                  y++;
               }
            } else {
                say("No Coin Prices In History For ETH");
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}



QString bpDatabase::getDefaultAccountId() {
    QString idResult = 0;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from accounts WHERE defaultAccount=1 and exchange='coinbase'");
            if (query.exec()) {
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  idResult = query.value(idVal).toString();
                  say("id: " + idResult);
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return idResult;
}

QString bpDatabase::getDefaultGdaxAccountId() {
    QString idResult = 0;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from gdaxAccounts WHERE defaultAccount=1 and exchange='GDAX'");
            if (query.exec()) {
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  idResult = query.value(idVal).toString();
                  say("id: " + idResult);
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return idResult;
}

void bpDatabase::getAutoSpotBuysForSaleList(QList<QString> *idList,QList<QString> *boughtAtList,QList<QString> *coinAmountList, QString coin) {
    QString idResult;
    QString batResult;
    QString amountResult;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from autoSpotTradeHistory where status='BOUGHT' AND type='BUY' AND coin='"+coin+"'");
            if (query.exec()) {
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  idResult = query.value(idVal).toString();
                  idList->append(idResult);

                  int batVal = query.record().indexOf("boughtAt");
                  batResult = query.value(batVal).toString();
                  boughtAtList->append(batResult);

                  int camVal = query.record().indexOf("coinAmount");
                  amountResult = query.value(camVal).toString();
                  coinAmountList->append(amountResult);
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

bool bpDatabase::createAccountsTable() {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           say("Database: CREATE connection ok.");
           QSqlQuery query;
           query.prepare("CREATE TABLE accounts (id INTEGER PRIMARY KEY AUTOINCREMENT,exchange varchar(64),apiKey varchar(512),apiSecret varchar(512),defaultAccount bool default 0, name VARCHAR(64) );");
           if(query.exec()) {
              retVal = true;
           } else {
              say("createAccountsTable() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

bool bpDatabase::createGdaxAccountsTable() {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           say("Database: CREATE connection ok.");
           QSqlQuery query;
           query.prepare("CREATE TABLE gdaxAccounts (id INTEGER PRIMARY KEY AUTOINCREMENT,exchange varchar(64),apiKey varchar(512),apiSecret varchar(512),defaultAccount bool default 0, name VARCHAR(64), passPhrase VARCHAR(2048) );");
           if(query.exec()) {
              retVal = true;
           } else {
              say("createGdaxAccountsTable() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

bool bpDatabase::createGdaxPriceHistoryTable() {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           say("Database: CREATE connection ok.");
           QSqlQuery query;
           query.prepare("CREATE TABLE gdaxPriceHistory (id INTEGER PRIMARY KEY AUTOINCREMENT,coin varchar(8),price varchar(256),ask varchar(256),bid varchar(256),ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL );");
           if(query.exec()) {
              retVal = true;
           } else {
              say("createGdaxPriceHistoryTable() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

//table: gdaxAutoTraderHistory
//   -> id, coin, type, status, amount, buyPrice,buyTotal, sellTarget, sellTotal, minProfitPct, minProfitUsd, timePlaced, timeBought, timeSellPlaced, timeSold
bool bpDatabase::createGdaxTraderHistoryTable() {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           say("Database: CREATE connection ok.");
           QSqlQuery query;
           query.prepare("CREATE TABLE gdaxAutoTraderHistory (id INTEGER PRIMARY KEY AUTOINCREMENT,coin varchar(8),type varchar(4),status varchar(16),"
                         "amount varchar(512),buyPrice varchar(24),buyTotal varchar(24),sellTarget varchar(24), sellTotal varchar(24),minProfitPct varchar(8),"
                         "minProfitUsd varchar(16), timePlaced TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,"
                         "timeBought TIMESTAMP DEFAULT NULL, timeSellPlaced TIMESTAMP DEFAULT NULL, timeSold TIMESTAMP DEFAULT NULL,orderId varchar(128) DEFAULT NULL );");
           if(query.exec()) {
              retVal = true;
           } else {
              say("createGdaxTraderHistoryTable() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

bool bpDatabase::createCbSpotPriceHistoryTable() {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           say("Database: CREATE connection ok.");
           QSqlQuery query;
           query.prepare("CREATE TABLE cbSpotPriceHistory (id INTEGER PRIMARY KEY AUTOINCREMENT,coin varchar(8),price varchar(256),ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL );");
           if(query.exec()) {
              retVal = true;
           } else {
              say("createCbSpotPriceHistoryTable() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

bool bpDatabase::createAutoSpotTradeHistoryTable() {
    bool retVal = false;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           say("Database: CREATE connection ok.");
           QSqlQuery query;
           query.prepare("CREATE TABLE autoSpotTradeHistory (id INTEGER PRIMARY KEY AUTOINCREMENT,coin varchar(8),type varchar(12),boughtAt varchar(256),soldAt varchar(256), status varchar(256),coinAmount varchar(256),ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL );");
           if(query.exec()) {
              retVal = true;
           } else {
              say("createAccountsTable() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}
//will need coin varchar(8),type varchar(12),boughtAt varchar(256),soldAt varchar(256), status varchar(256) ,amount varchar(256) when inserting

void bpDatabase::insertAutoSpotTrade( QString coin, QString type, QString boughtAt,QString soldAt,QString status,QString coinAmount) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("INSERT INTO autoSpotTradeHistory (coin,type,boughtAt,soldAt,status,coinAmount) VALUES ('" + coin + "','" + type + "','" + boughtAt + "','" + soldAt + "','" + status + "','" + coinAmount + "')");
           query.prepare(q);
           if(query.exec()) {
              say("insertAutoSpotTrade() Success");
           } else {
              say("insertAutoSpotTrade() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::updateAutoSpotTradeSoldAt( QString id, QString soldAt) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("UPDATE autoSpotTradeHistory SET soldAt='" + soldAt + "',status='SOLD' WHERE id="+ id );
           query.prepare(q);
           if(query.exec()) {
              say("updateAutoSpotTradeSoldAt() Success");
           } else {
              say("updateAutoSpotTradeSoldAt() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}


void bpDatabase::loadAccountByName(coinbaseAccount *target, QString accountName) {
    say(QString().setNum(target->mDefaultAccount) + "^" + accountName);
}

void bpDatabase::loadAccountById(coinbaseAccount *target, QString id) {
    target->mId = id;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from accounts WHERE id=" + id);
            if (query.exec()) {
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  QString idResult = query.value(idVal).toString();
                  int nameVal = query.record().indexOf("name");
                  target->mName = query.value(nameVal).toString();
                  int keyVal = query.record().indexOf("apiKey");
                  target->mApiKey = query.value(keyVal).toString();
                  int secVal = query.record().indexOf("apiSecret");
                  target->mApiSecret = query.value(secVal).toString();
                  int defVal = query.record().indexOf("defaultAccount");
                  target->mDefaultAccount = query.value(defVal).toBool();
                  int excVal = query.record().indexOf("exchange");
                  target->mExchange = query.value(excVal).toString();
                  say("id: " + target->mId);
                  say("name: " + target->mName);
                  say("Default?: " + QString().setNum(target->mDefaultAccount));
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::loadGdaxAccountById(gdaxAccount *target, QString id) {
    target->mId = id;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
            QSqlQuery query;
            query.prepare("select * from gdaxAccounts WHERE id=" + id);
            if (query.exec()) {
               while (query.next()) {
                  int idVal = query.record().indexOf("id");
                  QString idResult = query.value(idVal).toString();
                  int nameVal = query.record().indexOf("name");
                  target->mName = query.value(nameVal).toString();
                  int keyVal = query.record().indexOf("apiKey");
                  target->mApiKey = query.value(keyVal).toString();
                  int secVal = query.record().indexOf("apiSecret");
                  target->mApiSecret = query.value(secVal).toString();
                  int defVal = query.record().indexOf("defaultAccount");
                  target->mDefaultAccount = query.value(defVal).toBool();
                  int excVal = query.record().indexOf("exchange");
                  target->mExchange = query.value(excVal).toString();
                  int passVal = query.record().indexOf("passPhrase");
                  target->mPassPhrase = query.value(passVal).toString();
                  say("id: " + target->mId);
                  say("name: " + target->mName);
                  say("Default?: " + QString().setNum(target->mDefaultAccount));
               }
            }
        }
        Db.close();
    } //Db is gone
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::say(QString sayThis) {
    std::cout<<"("<<mPtrName.toStdString().c_str()<<") "<<sayThis.toStdString().c_str()<<std::endl;
}

bool bpDatabase::fileExists() {
    QFileInfo fCheck("bitProphet.dat");
    if (fCheck.exists() && fCheck.isFile()) {
        return true;
    } else {
        return false;
    }
}

void bpDatabase::insertAccount( QString name, QString apiKey, QString apiSecret, bool defaultAccount,  QString exchange) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           query.prepare("INSERT INTO accounts (exchange, apiKey, apiSecret,defaultAccount,name) VALUES ('" +
                         exchange + "','" + apiKey + "','" + apiSecret + "'," + QString().setNum(QVariant(defaultAccount).toInt()) +
                         ",'" + name + "')");
           if(query.exec()) {              
              say("insertAccount() Success");
           } else {
              say("insertAccount() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::insertGdaxAccount( QString name, QString apiKey, QString apiSecret, bool defaultAccount, QString passPhrase, QString exchange) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           query.prepare("INSERT INTO gdaxAccounts (exchange, apiKey, apiSecret,defaultAccount,name, passPhrase) VALUES ('" +
                         exchange + "','" + apiKey + "','" + apiSecret + "'," + QString().setNum(QVariant(defaultAccount).toInt()) +
                         ",'" + name + "','"+ passPhrase + "')");
           if(query.exec()) {
              say("insertGdaxAccount() Success");
           } else {
              say("insertGdaxAccount() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

int bpDatabase::getLastIdForTable(QString table) {
    int retVal = 0;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("select seq from sqlite_sequence where name='"+table+"'");
           std::cout<<q.toStdString().c_str()<<std::endl;
           query.prepare(q);
           if(query.exec()) {
               if ( query.next() )  {
                   int idVal = query.record().indexOf("seq");
                   QString idResult = query.value(idVal).toString();
                   retVal = idResult.toInt();
               } else {
                   say("getLastIdForTable() error:  " + query.lastError().text());
                }
           } else {
               say("getLastIdForTable() error:  " + query.lastError().text());
           }
         }
         Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

//id, coin, type, status, amount, buyPrice,buyTotal, sellTarget, sellTotal, minProfitPct, minProfitUsd, timePlaced, timeBought, timeSellPlaced, timeSold
void bpDatabase::insertGdaxAutoTrade( QString coin, QString type, QString status, QString amount, QString buyPrice, QString buyTotal, QString sellTarget, QString sellTotal, QString minProfitPct, QString minProfitUsd) {
    int retVal = 0;
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("INSERT INTO gdaxAutoTraderHistory (coin,type,status,amount,buyPrice,buyTotal, sellTarget, sellTotal, minProfitPct, minProfitUsd"
                     ") VALUES ('" +
                     coin + "','" + type + "','" + status + "','" + amount + "','" + buyPrice + "','" + buyTotal + "','" +sellTarget+"','"+sellTotal+"','" +
                     minProfitPct + "','" +minProfitUsd + "')");
           std::cout<<q.toStdString().c_str()<<std::endl;
           query.prepare(q);
           if(query.exec()) {
              say("insertgdaxAutoTraderHistory() Success");
           } else {
              say("insertgdaxAutoTraderHistory() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::getGdaxAutoBuysPlaced(QString coin,QList<QString> *buys) {
    //This is really for status == "placed2" (once the placement is verified via response)
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("select orderId from gdaxAutoTraderHistory where coin='"+coin+"' AND status='placed2' AND type='BUY'");
           query.prepare(q);
           if(query.exec()) {
               while ( query.next() )  {
                   int idVal = query.record().indexOf("orderId");
                   QString idResult = query.value(idVal).toString();
                   buys->append(idResult);
               }
           } else {
               say("getGdaxAutoBuysPlaced() error:  " + query.lastError().text());
           }
         }
         Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::getGdaxAutoSellsPosted(QString coin,QList<QString> *sells) {
    //This is really for status == "placed2" (once the placement is verified via response)
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("select orderId from gdaxAutoTraderHistory where coin='"+coin+"' AND status='posted2' AND type='SELL'");
           query.prepare(q);
           if(query.exec()) {
               while ( query.next() )  {
                   int idVal = query.record().indexOf("orderId");
                   QString idResult = query.value(idVal).toString();
                   sells->append(idResult);
               }
           } else {
               say("getGdaxAutoSellsPosted() error:  " + query.lastError().text());
           }
         }
         Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}


QString bpDatabase::getGdaxAutoTradeIdByOrderId(QString orderId) {
    //This is really for status == "placed2" (once the placement is verified via response)
    QString retVal("");
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("select id from gdaxAutoTraderHistory where orderId='"+orderId+"'");
           query.prepare(q);
           if(query.exec()) {
               while ( query.next() )  {
                   int idVal = query.record().indexOf("id");
                   retVal = query.value(idVal).toString();
               }
           } else {
               say("getGdaxAutoTradeIdByOrderId() error:  " + query.lastError().text());
           }
         }
         Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}

QString bpDatabase::getGdaxAutoTradeHistoryValueById(QString id,QString column) {
    QString retVal("");
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("select "+column+" from gdaxAutoTraderHistory where id='"+id+"'");
           query.prepare(q);
           if(query.exec()) {
               while ( query.next() )  {
                   int idVal = query.record().indexOf(column);
                   retVal = query.value(idVal).toString();
               }
           } else {
               say("getGdaxAutoTradeValueById() error:  " + query.lastError().text());
           }
         }
         Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return retVal;
}


void bpDatabase::addToCbSpotPriceHistory( QString coin, QString price ) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           query.prepare("INSERT INTO cbSpotPriceHistory (coin, price) VALUES ('" + coin + "','" + price + "')");
           if(query.exec()) {
              //say("addToCbSpotPriceHistory() Success");
           } else {
              say("addToCbSpotPriceHistory() Error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::updateRowById( QString id, QString table, QString column, QString newValue ) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           QString q("UPDATE " + table + " SET "+column+"='" + newValue + "' WHERE id=" + id );
           query.prepare(q);
           say(q);
           if(query.exec()) {
              //say("updateRowById() Success");
           } else {
              say("updateRowById() Error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::addToGdaxPriceHistory( QString coin, QString price,QString ask, QString bid ) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           query.prepare("INSERT INTO gdaxPriceHistory (coin, price,ask,bid) VALUES ('" + coin + "','" + price + "','" + ask + "','" + bid + "' )");
           if(query.exec()) {
              //say("addToCbSpotPriceHistory() Success");
           } else {
              say("addToGdaxPriceHistory() Error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::deleteAccount(QString id ) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           query.prepare("DELETE FROM accounts WHERE id=" + QVariant(id).toString());
           if(query.exec()) {
              say("deleteAccount() Success");
           } else {
              say("deleteAccount() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void bpDatabase::deleteGdaxAccount(QString id ) {
    {
        QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
        Db.setDatabaseName("bitProphet.dat");
        if (!Db.open()) {
           say("Error: connecting to database failed!");
        } else {
           //say("Database: connection ok.");
           QSqlQuery query;
           query.prepare("DELETE FROM gdaxAccounts WHERE id=" + QVariant(id).toString());
           if(query.exec()) {
              say("deleteGdaxAccount() Success");
           } else {
              say("deleteGdaxAccount() error:  " + query.lastError().text());
           }
        }
        Db.close();
    }
    QSqlDatabase::removeDatabase("bitProphet.dat");
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

/////////
// Slots
/////////
