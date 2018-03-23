#include "bppricegraph.h"

bpPriceGraph::bpPriceGraph(bitProphet *parent) : mLineSeriesList(NULL), mChart(NULL),mChartView(NULL) {
    mParent = parent;
}


bpPriceGraph::~bpPriceGraph() {
    // SPECIAL ->>   //if ( mLineSeriesList != NULL )
    if ( mChart != NULL ) { mChart->deleteLater(); }
    if (mChartView != NULL ) { mChartView->deleteLater(); }
}

