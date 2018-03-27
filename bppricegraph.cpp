#include "bppricegraph.h"

// 2018-03-24T00:22:52.468607Z <-- Server(gdax fills) given Times
// YYYY-MM-DD 00:00:00 <--- SQLITE DB Times
// Desired output timeformats on graphs (and otherwise) ->> MM-DD-YYYY 00:00:00


bpPriceGraph::bpPriceGraph(bpWindow *parent,QString chartName,QWidget *WidgetParent) : mLineSeriesList(NULL), mChart(NULL),mChartView(NULL) {
    mParent = parent;
    mName = chartName;
    //Data Series LIST-OF
    mLineSeriesList = new QList<QSplineSeries*>();
    mLineSeriesList->append(new QSplineSeries());
    mTimeTool = new timeTool(this);
    mWidgetParent = WidgetParent;
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
    std::cout<<"|bpPriceGraph| "<<sayThis.toStdString()<<std::endl;
}

void bpPriceGraph::loadPrices(QList<QString> newSeries,QList<QString> timeSeries) {
    say("Loading...");

    for(int z=0;z<newSeries.count();z++){        
        //std::cout<<std::endl<<timeSeries.at(z).toStdString()<<std::endl;
        QDateTime dt = QDateTime::fromString(timeSeries.at(z),"yyyy-MM-dd hh:mm:ss");
        mLineSeriesList->at(mLineSeriesList->count()-1)->append(QDateTime().fromString(timeSeries.at(z),"yyyy-MM-dd hh:mm:ss").toMSecsSinceEpoch(),newSeries.at(z).toDouble());
        //say("Plotting y:" + newSeries.at(z) + " x:" + dt.toString() + " Valid: " + QString().setNum(dt.isValid()) );
    }
    mChart = new QChart();
    mChart->legend()->hide();
    mChart->addSeries(mLineSeriesList->at(mLineSeriesList->count()-1));
    mChart->createDefaultAxes();
    mChart->removeAxis(mChart->axisX());
    mChart->setTitle(mName);
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm:ss");
    mChart->addAxis(axisX, Qt::AlignBottom);
    mLineSeriesList->at(mLineSeriesList->count()-1)->attachAxis(axisX);
    //mChart->axisX()->setRange(0000,2459.59);

    mChartView = new QChartView(mChart,mWidgetParent);
    mChartView->setRenderHint(QPainter::Antialiasing);
    //mChartView->setGeometry(QRect(0,0,600,400));
    //mChart->setGeometry(mParent->getBtcUsdPriceLabel()->geometry().x(),mParent->getBtcUsdPriceLabel()->geometry().y(),400,400);

    mChartView->setGeometry(0,16,mWidgetParent->geometry().width()*1.8,350);
    say(mName + " Graph Loaded.");
    mChartView->show();
}

void bpPriceGraph::reloadPrices(QList<QString> newSeries,QList<QString> timeSeries,int seriesIndex) {
    mLineSeriesList->at(seriesIndex)->clear();
    for(int z=0;z<newSeries.count();z++){
        QDateTime dt = QDateTime::fromString(timeSeries.at(z),"yyyy-MM-dd hh:mm:ss");
        mLineSeriesList->at(seriesIndex)->append(QDateTime().fromString(timeSeries.at(z),"yyyy-MM-dd hh:mm:ss").toMSecsSinceEpoch(),newSeries.at(z).toDouble());
        //say("Plotting y:" + newSeries.at(z) + " x:" + mParent->mTimeTool->getHourFromSqlTimeStamp(timeSeries.at(z)));
    }
    //mChart->axisX()->setRange(mTimeTool->getHourFromSqlTimeStamp(timeSeries.at(0)).toDouble(),mTimeTool->getHourFromSqlTimeStamp(timeSeries.at(newSeries.count()-1)).toDouble());
    mChart->axisY()->setRange(lowestPrice(newSeries).toDouble()-0.05,highestPrice(newSeries).toDouble()+0.05);
    //mChart->createDefaultAxes();
    mChartView->repaint();
}

QString bpPriceGraph::lowestPrice(QList<QString> pList) {
    QString lowest = pList.at(0);
    for(int c=0;c<pList.count();c++){
        if ( pList.at(c).toDouble() < lowest.toDouble() ) {
            lowest = pList.at(c);
        }
    }
    return lowest;
}

QString bpPriceGraph::highestPrice(QList<QString> pList) {
    QString highest = pList.at(0);
    for(int c=0;c<pList.count();c++){
        if ( pList.at(c).toDouble() > highest.toDouble() ) {
            highest = pList.at(c);
        }
    }
    return highest;
}
