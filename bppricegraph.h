#ifndef BPPRICEGRAPH_H
#define BPPRICEGRAPH_H

#include <QWidget>
#include "bitprophet.h"
#include <QChart>
#include <QLineSeries>
#include <QChartView>

class bitProphet;

class bpPriceGraph : public QWidget {
    Q_OBJECT
public:
    explicit bpPriceGraph(bitProphet *parent,QString chartName);
    ~bpPriceGraph();    
    void loadPrices(QList<QString> newSeries,QList<QString> timeSeries);
signals:

public slots:
private:
    bitProphet *mParent;
    QList<QSplineSeries*> *mLineSeriesList;
    QChart *mChart;
    QChartView *mChartView;
    void say(QString sayThis);
    QString mName;
};

#endif // BPPRICEGRAPH_H
