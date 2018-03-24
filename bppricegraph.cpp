#include "bppricegraph.h"

// 2018-03-24T00:22:52.468607Z <-- Server(gdax fills) given Times
// YYYY-MM-DD 00:00:00 <--- SQLITE DB Times
// Desired output timeformats on graphs (and otherwise) ->> MM-DD-YYYY 00:00:00


bpPriceGraph::bpPriceGraph(bitProphet *parent,QString chartName) : mLineSeriesList(NULL), mChart(NULL),mChartView(NULL) {
    mParent = parent;
    mName = chartName;
    //Data Series LIST-OF
    mLineSeriesList = new QList<QSplineSeries*>();
    mLineSeriesList->append(new QSplineSeries());
    //chart, singular

    //cView, singular

    say(mName + " Graph Created.");
}


bpPriceGraph::~bpPriceGraph() {
    if ( mLineSeriesList->at(0) != NULL ) { mLineSeriesList->at(0)->deleteLater(); }
    if ( mChart != NULL ) { mChart->deleteLater(); }
    if ( mChartView != NULL ) { mChartView->deleteLater(); }
}

void bpPriceGraph::say(QString sayThis) {
    mParent->say("|bpPriceGraph| "+ sayThis);
}

void bpPriceGraph::loadPrices(QList<QString> newSeries,QList<QString> timeSeries) {
    for(int z=0;z<newSeries.count();z++){
        mLineSeriesList->at(mLineSeriesList->count()-1)->append(mParent->mTimeTool->getHourFromSqlTimeStamp(timeSeries.at(z)).toDouble(),newSeries.at(z).toDouble());
        //say("Plotting y:" + newSeries.at(z) + " x:" + mParent->mTimeTool->getHourFromSqlTimeStamp(timeSeries.at(z)));
    }
    mChart = new QChart();
    mChart->legend()->hide();
    mChart->addSeries(mLineSeriesList->at(mLineSeriesList->count()-1));
    mChart->createDefaultAxes();
    mChart->setTitle(mName);
    mChart->axisX()->setRange(0000,2459.59);
    mChartView = new QChartView(mChart);
    mChartView->setRenderHint(QPainter::Antialiasing);
    mChartView->setGeometry(QRect(0,0,600,400));
    say(mName + " Graph Loaded.");
    mChartView->show();
}


