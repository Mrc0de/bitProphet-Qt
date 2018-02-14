#include "gdaxaccountsetupwindow.h"
#include "ui_gdaxaccountsetupwindow.h"

gdaxAccountSetupWindow::gdaxAccountSetupWindow(bpWindow *parent) : QDialog(parent), ui(new Ui::gdaxAccountSetupWindow), mAddAccountWin(NULL) {
    ui->setupUi(this);
    mParent = parent;
//    setWindowFlag(Qt::WindowCloseButtonHint,false);
//    setWindowFlag(Qt::WindowContextHelpButtonHint,false);
    mParent->hide();
    loadAccountList();
    //Bind Buttons
    connect(ui->mAddButton,SIGNAL(clicked(bool)),this,SLOT(addAccountClicked()));
    connect(ui->mDoneButton,SIGNAL(clicked(bool)),this,SLOT(doneClicked()));
    connect(ui->mRemoveButton,SIGNAL(clicked(bool)),this,SLOT(deleteAccountClicked()));
}

gdaxAccountSetupWindow::~gdaxAccountSetupWindow() {
    if ( mAddAccountWin != NULL ) { delete mAddAccountWin; }
    delete ui;
}

///////////
// METHODS
///////////

void gdaxAccountSetupWindow::killSetupAddWindow() {
    delete mAddAccountWin;
    mAddAccountWin = NULL;
}

void gdaxAccountSetupWindow::loadAccountList() {
    ui->mAccountList->clear();
    mDb = new bpDatabase(this);
    QList<QString> list;
    if ( mDb->hasTable("gdaxAccounts") ) {
        list = mDb->getGdaxAccountList();
    }
    delete mDb;
    for(int counter=0;counter<list.count();counter++) {
        ui->mAccountList->addItem(list.at(counter));
    }
}

/////////
// SLOTS
/////////

void gdaxAccountSetupWindow::addAccountClicked() {
    if ( mAddAccountWin != NULL ) { delete mAddAccountWin; }
    mAddAccountWin = new gdaxSetupAddWindow(this);
    mAddAccountWin->show();
}

void gdaxAccountSetupWindow::deleteAccountClicked() {
    mDb = new bpDatabase(this);
    if ( ui->mAccountList->currentItem() != NULL ) {
        QString delId = ui->mAccountList->currentItem()->text();
        std::cout<<delId.toStdString().c_str()<<std::endl;
        std::string dId(delId.toStdString());
        delId = dId.substr(0,dId.find(" - ",0)).c_str();
        std::cout<<"#### "<<dId<<std::endl;
        if ( mDb->hasTable("gdaxAccounts") ) {
            mDb->deleteGdaxAccount(delId);
        }
    }
    delete mDb;
    loadAccountList();
}

void gdaxAccountSetupWindow::doneClicked() {
    if ( mAddAccountWin != NULL ) { delete mAddAccountWin; }
    mParent->show();
    mParent->killGdaxAccSetupWindow();
}
