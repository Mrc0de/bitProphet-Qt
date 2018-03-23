#-------------------------------------------------
#
# Project created by QtCreator 2017-07-31T23:23:50
#
#-------------------------------------------------

QT       += core gui sql network charts websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bitProphet-Qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        bpwindow.cpp \
    bitprophet.cpp \
    bpdatabase.cpp \
    coinbaseaccount.cpp \
    coinbaseaccountsetupwindow.cpp \
    coinbasesetupaddwindow.cpp \
    cbapihandler.cpp \
    cbapirequest.cpp \
    cbapiresponse.cpp \
    coinbasewallet.cpp \
    cbwallettable.cpp \
    bpsplinechart.cpp \
    spotprice.cpp \
    coinbasepaymentmethod.cpp \
    cbtransaction.cpp \
    cbtransactiontable.cpp \
    cbautospottrader.cpp \
    gdaxapihandler.cpp \
    gdaxaccount.cpp \
    gdaxaccountsetupwindow.cpp \
    gdaxsetupaddwindow.cpp \
    gdaxapirequest.cpp \
    gdaxapiresponse.cpp \
    gdaxwallet.cpp \
    gdaxwallettable.cpp \
    gdaxautotrader.cpp \
    gdaxwebsockethandler.cpp \
    timetool.cpp \
    bppricegraph.cpp

HEADERS += \
        bpwindow.h \
    bitprophet.h \
    bpdatabase.h \
    coinbaseaccount.h \
    coinbaseaccountsetupwindow.h \
    coinbasesetupaddwindow.h \
    cbapihandler.h \
    cbapirequest.h \
    cbapiresponse.h \
    coinbasewallet.h \
    cbwallettable.h \
    bpsplinechart.h \
    spotprice.h \
    coinbasepaymentmethod.h \
    cbtransaction.h \
    cbtransactiontable.h \
    cbautospottrader.h \
    gdaxapihandler.h \
    gdaxaccount.h \
    gdaxaccountsetupwindow.h \
    gdaxsetupaddwindow.h \
    gdaxapirequest.h \
    gdaxapiresponse.h \
    gdaxwallet.h \
    gdaxwallettable.h \
    gdaxautotrader.h \
    gdaxwebsockethandler.h \
    timetool.h \
    bppricegraph.h

FORMS += \
        bpwindow.ui \
    coinbaseaccountsetupwindow.ui \
    coinbasesetupaddwindow.ui \
    gdaxaccountsetupwindow.ui \
    gdaxsetupaddwindow.ui
