#include "gdaxsetupaddwindow.h"
#include "ui_gdaxsetupaddwindow.h"

gdaxSetupAddWindow::gdaxSetupAddWindow(gdaxAccountSetupWindow *parent) :  QDialog(parent), ui(new Ui::gdaxSetupAddWindow), mDb(NULL) {
    ui->setupUi(this);
    mParent = parent;
//    setWindowFlag(Qt::WindowCloseButtonHint,false);
//    setWindowFlag(Qt::WindowContextHelpButtonHint ,false);
    mParent->hide();
    connect(ui->mCancelButton,SIGNAL(clicked(bool)),this,SLOT(doneClicked()));
    connect(ui->mSaveButton,SIGNAL(clicked(bool)),this,SLOT(saveClicked()));
}

gdaxSetupAddWindow::~gdaxSetupAddWindow() {
    if (mDb != NULL ) { delete mDb; }
    delete ui;
}

///////////
// Methods
///////////



/////////
// Slots
/////////

void gdaxSetupAddWindow::doneClicked() {
    mParent->show();
    mParent->killSetupAddWindow();
}

void gdaxSetupAddWindow::saveClicked() {
    if ( ui->mAccountNameEntry->text().length() > 0 && ui->mApiKeyEntry->text().length() > 0 && ui->mApiSecretEntry->text().length() > 0 && ui->mApiPassPhrase->text().length() > 0) {
        //good input :)
        bool check = false;
        if ( ui->mDefaultCheckbox->isChecked() ) { check = true; }
        mDb = new bpDatabase(this);
        mDb->insertGdaxAccount(ui->mAccountNameEntry->text(),ui->mApiKeyEntry->text(),ui->mApiSecretEntry->text(),check,ui->mApiPassPhrase->text());
        //die when done
        mParent->show();
        mParent->loadAccountList();
        mParent->killSetupAddWindow();
    } else {
        //BAD INPUTZ!@
        QMessageBox::warning(this,"BAD INPUT!","Please Fill-In All Fields.");
    }
}
