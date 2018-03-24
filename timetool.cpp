#include "timetool.h"

// GDAX Fill 'created_at' date style
// 2018-03-22T05:10:24.498419Z
// Need to transpose (maybe) and return more suitable format (for whatever task)

// Additional uses/utils needed.. time/date comparison,evaluation functions

timeTool::timeTool(QObject *parent) : QObject(parent) {

}

timeTool::~timeTool() {

}

QString timeTool::getCurrentDateTimeForSql() {
    return QDate::currentDate().toString("yyyy-MM-dd ") + QTime::currentTime().toString("hh:mm:ss");
}

QString timeTool::getHourFromSqlTimeStamp(QString sqlTimeStamp) {
    std::cout<<sqlTimeStamp.toStdString()<<std::endl;
    std::cout<<sqlTimeStamp.mid(sqlTimeStamp.indexOf(":")-2,2).toStdString()<<std::endl;
    QString restVal;
    QString secVal;
    restVal = sqlTimeStamp.mid(sqlTimeStamp.indexOf(":")+1);
    secVal = restVal.mid(3,2);
    restVal = restVal.mid(0,2) +"."+secVal;
    std::cout<<restVal.toStdString()<<std::endl;
    return QString(sqlTimeStamp.mid(sqlTimeStamp.indexOf(":")-2,2) + restVal);
}
