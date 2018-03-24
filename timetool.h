#ifndef TIMETOOL_H
#define TIMETOOL_H

#include <QObject>
#include <QDate>
#include <QTime>
#include <iostream>
class timeTool : public QObject {
    Q_OBJECT
public:
    explicit timeTool(QObject *parent = nullptr);
    ~timeTool();
    QString getCurrentDateTimeForSql();
    QString getHourFromSqlTimeStamp(QString sqlTimeStamp);
signals:

public slots:

private:

};

#endif // TIMETOOL_H
