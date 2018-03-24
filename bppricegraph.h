#ifndef BPPRICEGRAPH_H
#define BPPRICEGRAPH_H

#include <QWidget>
#include "bitprophet.h"
#include "bpwindow.h"
#include "timetool.h"
#include <QChart>
#include <QLineSeries>
#include <QChartView>

class bitProphet;
class bpWindow;
class timeTool;

class bpPriceGraph : public QWidget {
    Q_OBJECT
public:
    explicit bpPriceGraph(bpWindow *parent,QString chartName,QWidget *WidgetParent);
    ~bpPriceGraph();    
    void loadPrices(QList<QString> newSeries,QList<QString> timeSeries);
signals:

public slots:
private:
    bpWindow *mParent;
    QList<QSplineSeries*> *mLineSeriesList;
    QChart *mChart;
    QChartView *mChartView;
    void say(QString sayThis);
    QString mName;
    timeTool *mTimeTool;
    QWidget *mWidgetParent;
};

#endif // BPPRICEGRAPH_H
