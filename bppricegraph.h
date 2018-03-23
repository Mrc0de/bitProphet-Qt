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
    explicit bpPriceGraph(bitProphet *parent);
    ~bpPriceGraph();
signals:

public slots:
private:
    bitProphet *mParent;
    QList<QLineSeries*> *mLineSeriesList;
    QChart *mChart;
    QChartView *mChartView;

};

#endif // BPPRICEGRAPH_H
