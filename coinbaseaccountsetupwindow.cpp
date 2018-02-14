#include "coinbaseaccountsetupwindow.h"
#include "ui_coinbaseaccountsetupwindow.h"

coinbaseAccountSetupWindow::coinbaseAccountSetupWindow(QWidget *parent) :
    QDialog(parent),  ui(new Ui::coinbaseAccountSetupWindow), mAddAccountWin(NULL) {
    ui->setupUi(this);    
    mParent = reinterpret_cast<bpWindow*>(parent);
//    setWindowFlag(Qt::WindowCloseButtonHint,false);
//    setWindowFlag(Qt::WindowContextHelpButtonHint,false);
    mParent->hide();
    loadAccountList();
    //Bind Buttons
    connect(ui->mAddButton,SIGNAL(clicked(bool)),this,SLOT(addAccountClicked()));
    connect(ui->mDoneButton,SIGNAL(clicked(bool)),this,SLOT(doneClicked()));
    connect(ui->mRemoveButton,SIGNAL(clicked(bool)),this,SLOT(deleteAccountClicked()));
}

coinbaseAccountSetupWindow::~coinbaseAccountSetupWindow() {
    if ( mAddAccountWin != NULL ) { delete mAddAccountWin; }
    delete ui;
}

///////////
// METHODS
///////////

void coinbaseAccountSetupWindow::killSetupAddWindow() {
    delete mAddAccountWin;
    mAddAccountWin = NULL;
}

void coinbaseAccountSetupWindow::loadAccountList() {
    ui->mAccountList->clear();
    mDb = new bpDatabase(this);
    QList<QString> list;
    if ( mDb->hasAccountsTable() ) {
        list = mDb->getAccountList();
    }
    delete mDb;
    for(int counter=0;counter<list.count();counter++) {
        ui->mAccountList->addItem(list.at(counter));
    }
}

/////////
// Slots
/////////

void coinbaseAccountSetupWindow::addAccountClicked() {
    if ( mAddAccountWin != NULL ) { delete mAddAccountWin; }
    mAddAccountWin = new coinbaseSetupAddWindow(this);
    mAddAccountWin->show();
}

void coinbaseAccountSetupWindow::deleteAccountClicked() {
    mDb = new bpDatabase(this);
    QString delId = ui->mAccountList->currentItem()->text();
    std::cout<<delId.toStdString().c_str()<<std::endl;
    std::string dId(delId.toStdString());
    delId = dId.substr(0,dId.find(" - ",0)).c_str();
    std::cout<<"#### "<<dId<<std::endl;
    if ( mDb->hasAccountsTable() ) {
        mDb->deleteAccount(delId);
    }
    delete mDb;
    loadAccountList();
}

void coinbaseAccountSetupWindow::doneClicked() {
    if ( mAddAccountWin != NULL ) { delete mAddAccountWin; }
    mParent->show();
    mParent->killAccSetupWindow();
}


