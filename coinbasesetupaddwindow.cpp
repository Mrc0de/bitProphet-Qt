#include "coinbasesetupaddwindow.h"
#include "ui_coinbasesetupaddwindow.h"
#include <QMessageBox>

coinbaseSetupAddWindow::coinbaseSetupAddWindow(QWidget *parent) :  QDialog(parent), ui(new Ui::coinbaseSetupAddWindow),mDb(NULL) {
    ui->setupUi(this);
    mParent = reinterpret_cast<coinbaseAccountSetupWindow*>(parent);
//    setWindowFlag(Qt::WindowCloseButtonHint,false);
//    setWindowFlag(Qt::WindowContextHelpButtonHint ,false);
    mParent->hide();
    connect(ui->mCancelButton,SIGNAL(clicked(bool)),this,SLOT(doneClicked()));
    connect(ui->mSaveButton,SIGNAL(clicked(bool)),this,SLOT(saveClicked()));
}

coinbaseSetupAddWindow::~coinbaseSetupAddWindow() {
    if (mDb != NULL ) { delete mDb; }
    delete ui;
}

void coinbaseSetupAddWindow::say(QString sayThis) {
    std::cout<<"addWindow: "<<sayThis.toStdString().c_str()<<std::endl;
}

/////////
// Slots
/////////

void coinbaseSetupAddWindow::doneClicked() {
    mParent->show();
    mParent->killSetupAddWindow();
}

void coinbaseSetupAddWindow::saveClicked() {
    if ( ui->mAccountNameEntry->text().length() > 0 && ui->mApiKeyEntry->text().length() > 0 && ui->mApiSecretEntry->text().length() > 0 ) {
        //good input :)
        bool check = false;
        if ( ui->mDefaultCheckbox->isChecked() ) { check = true; }
        mDb = new bpDatabase(this);
        mDb->insertAccount(ui->mAccountNameEntry->text(),ui->mApiKeyEntry->text(),ui->mApiSecretEntry->text(),check);
        //die when done
        mParent->show();
        mParent->loadAccountList();
        mParent->killSetupAddWindow();
    } else {
        //BAD INPUTZ!@
        QMessageBox::warning(this,"BAD INPUT!","Please Fill-In All Fields.");
    }
}
