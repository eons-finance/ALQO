// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/settings/settingswidget.h"
#include "qt/settings/forms/ui_settingswidget.h"
#include "qt/settings/settingsbackupwallet.h"
#include "qt/settings/settingsbittoolwidget.h"
#include "qt/settings/settingswalletrepairwidget.h"
#include "qt/settings/settingswalletoptionswidget.h"
#include "qt/settings/settingsmainoptionswidget.h"
#include "qt/settings/settingsdisplayoptionswidget.h"
#include "qt/settings/settingsmultisendwidget.h"
#include "qt/settings/settingsinformationwidget.h"
#include "qt/settings/settingsconsolewidget.h"
#include "qt/qtutils.h"
#include "qt/defaultdialog.h"
#include "optionsmodel.h"
#include "clientmodel.h"
#include "utilitydialog.h"
#include "wallet/wallet.h"
SettingsWidget::SettingsWidget(ALQOGUI* parent) :
    PWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    this->setStyleSheet(parent->styleSheet());

    ui->verticalLayout->setAlignment(Qt::AlignTop);
    
    //setCssProperty(ui->scrollArea, "dash-frame");
   // setCssProperty(ui->stackedWidgetContainer, "dash-frame");

    /* Light Font */
    //QFont fontLight;
    //fontLight.setWeight(QFont::Light);

    /* Title */
    options = {
        ui->pushButtonBackup,
        ui->pushButtonMultisend,
        ui->pushButtonMain,
        ui->pushButtonWallet,
        ui->pushButtonDisplay,
        ui->pushButtonBip38,
        ui->pushButtonSignVerify,
        ui->pushButtonHelp,
        ui->pushButtonInformation,
        ui->pushButtonConsole,
        ui->pushButtonRepair,
    };

    ui->pushButtonBackup->setChecked(true);

    settingsBackupWallet = new SettingsBackupWallet(window, this);
    settingsBitToolWidget = new SettingsBitToolWidget(window, this);
    settingsSingMessageWidgets = new SettingsSignMessageWidgets(window, this);
    settingsWalletRepairWidget = new SettingsWalletRepairWidget(window, this);
    settingsWalletOptionsWidget = new SettingsWalletOptionsWidget(window, this);
    settingsMainOptionsWidget = new SettingsMainOptionsWidget(window, this);
    settingsDisplayOptionsWidget = new SettingsDisplayOptionsWidget(window, this);
    settingsMultisendWidget = new SettingsMultisendWidget(this);
    settingsInformationWidget = new SettingsInformationWidget(window, this);
    settingsConsoleWidget = new SettingsConsoleWidget(window, this);
    ui->stackedWidgetContainer->setSpeed(300);
    ui->stackedWidgetContainer->addWidget(settingsBackupWallet);
    ui->stackedWidgetContainer->addWidget(settingsBitToolWidget);
    ui->stackedWidgetContainer->addWidget(settingsSingMessageWidgets);
    ui->stackedWidgetContainer->addWidget(settingsWalletRepairWidget);
    ui->stackedWidgetContainer->addWidget(settingsWalletOptionsWidget);
    ui->stackedWidgetContainer->addWidget(settingsMainOptionsWidget);
    ui->stackedWidgetContainer->addWidget(settingsDisplayOptionsWidget);
    ui->stackedWidgetContainer->addWidget(settingsMultisendWidget);
    ui->stackedWidgetContainer->addWidget(settingsInformationWidget);
    ui->stackedWidgetContainer->addWidget(settingsConsoleWidget);
    ui->stackedWidgetContainer->setCurrentWidget(settingsBackupWallet);

    // File Section
    connect(ui->pushButtonBackup, SIGNAL(clicked()), this, SLOT(onBackupWalletClicked()));
    connect(ui->pushButtonMultisend, SIGNAL(clicked()), this, SLOT(onMultisendClicked()));

    // Options
    connect(ui->pushButtonMain, SIGNAL(clicked()), this, SLOT(onMainOptionsClicked()));
    connect(ui->pushButtonWallet, SIGNAL(clicked()), this, SLOT(onWalletOptionsClicked()));
    connect(ui->pushButtonDisplay, SIGNAL(clicked()), this, SLOT(onDisplayOptionsClicked()));

    // Configuration
    connect(ui->pushButtonBip38, SIGNAL(clicked()), this, SLOT(onBipToolClicked()));
    connect(ui->pushButtonSignVerify, SIGNAL(clicked()), this, SLOT(onSignMessageClicked()));

    // Tools
    connect(ui->pushButtonInformation, SIGNAL(clicked()), this, SLOT(onInformationClicked()));
    connect(ui->pushButtonConsole, SIGNAL(clicked()), this, SLOT(onDebugConsoleClicked()));
    ui->pushButtonConsole->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_C));
    connect(ui->pushButtonRepair, SIGNAL(clicked()), this, SLOT(onWalletRepairClicked()));

    // Help
    connect(ui->pushButtonHelp, SIGNAL(clicked()), this, SLOT(onAboutClicked()));

    // Get restart command-line parameters and handle restart
    connect(settingsWalletRepairWidget, &SettingsWalletRepairWidget::handleRestart, [this](QStringList arg){emit handleRestart(arg);});

    connect(settingsBackupWallet,&SettingsBackupWallet::message,this, &SettingsWidget::message);
    connect(settingsBackupWallet, &SettingsBackupWallet::showHide, this, &SettingsWidget::showHide);
    connect(settingsBackupWallet, &SettingsBackupWallet::execDialog, this, &SettingsWidget::execDialog);
    connect(settingsMultisendWidget, &SettingsMultisendWidget::showHide, this, &SettingsWidget::showHide);
    connect(settingsMultisendWidget, &SettingsMultisendWidget::message, this, &SettingsWidget::message);
    connect(settingsMainOptionsWidget, &SettingsMainOptionsWidget::message, this, &SettingsWidget::message);
    connect(settingsDisplayOptionsWidget, &SettingsDisplayOptionsWidget::message, this, &SettingsWidget::message);
    connect(settingsWalletOptionsWidget, &SettingsWalletOptionsWidget::message, this, &SettingsWidget::message);

    /* Widget-to-option mapper */
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setOrientation(Qt::Vertical);
}

void SettingsWidget::loadClientModel(){
    if(clientModel) {
        this->settingsInformationWidget->setClientModel(this->clientModel);
        this->settingsConsoleWidget->setClientModel(this->clientModel);

        OptionsModel *optionsModel = this->clientModel->getOptionsModel();
        if (optionsModel) {
            mapper->setModel(optionsModel);
            setMapper();
            mapper->toFirst();
            settingsMainOptionsWidget->setClientModel(clientModel);
            settingsDisplayOptionsWidget->setClientModel(clientModel);
            settingsWalletOptionsWidget->setClientModel(clientModel);
            /* keep consistency for action triggered elsewhere */
            //connect(optionsModel, SIGNAL(hideOrphansChanged(bool)), this, SLOT(updateHideOrphans(bool)));

            // TODO: Connect show restart needed and apply changes.
        }
    }
}

void SettingsWidget::loadWalletModel(){
    this->settingsBackupWallet->setWalletModel(this->walletModel);
    this->settingsSingMessageWidgets->setWalletModel(this->walletModel);
    this->settingsBitToolWidget->setWalletModel(this->walletModel);
    this->settingsMultisendWidget->setWalletModel(this->walletModel);
    this->settingsDisplayOptionsWidget->setWalletModel(this->walletModel);
}

void SettingsWidget::onResetAction(){
    if (walletModel) {
        // confirmation dialog
        if (!ask(tr("Confirm options reset"), tr("Client restart required to activate changes.") + "<br><br>" + tr("Client will be shutdown, do you want to proceed?")))
            return;

        /* reset all options and close GUI */
        this->clientModel->getOptionsModel()->Reset();
        QApplication::quit();
    }
}

void SettingsWidget::onSaveOptionsClicked(){
    if(mapper->submit()) {
        pwalletMain->MarkDirty();
        if (this->clientModel->getOptionsModel()->isRestartRequired()) {
            bool fAcceptRestart = openStandardDialog(tr("Restart required"), tr("Your wallet needs to be restarted to apply the changes\n"), tr("Restart Now"), tr("Restart Later"));

            if (fAcceptRestart) {
                // Get command-line arguments and remove the application name
                QStringList args = QApplication::arguments();
                args.removeFirst();

                // Remove existing repair-options
                args.removeAll(SALVAGEWALLET);
                args.removeAll(RESCAN);
                args.removeAll(ZAPTXES1);
                args.removeAll(ZAPTXES2);
                args.removeAll(UPGRADEWALLET);
                args.removeAll(REINDEX);

                emit handleRestart(args);
            } else {
                inform(tr("Options will be applied on next wallet restart"));
            }
        } else {
            inform(tr("Options stored"));
        }
    } else {
        inform(tr("Options store failed"));
    }
}

void SettingsWidget::onBackupWalletClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsBackupWallet, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonBackup);
}

void SettingsWidget::onSignMessageClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsSingMessageWidgets, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonSignVerify);
}

void SettingsWidget::onBipToolClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsBitToolWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonBip38);
}

void SettingsWidget::onMultisendClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsMultisendWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonMultisend);
}


void SettingsWidget::onMainOptionsClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsMainOptionsWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonMain);
}

void SettingsWidget::onWalletOptionsClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsWalletOptionsWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonWallet);
}

void SettingsWidget::onDisplayOptionsClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsDisplayOptionsWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonDisplay);
}


void SettingsWidget::onInformationClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsInformationWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonInformation);
}

void SettingsWidget::showDebugConsole(){
    ui->pushButtonConsole->setChecked(true);
    onDebugConsoleClicked();
}

void SettingsWidget::onDebugConsoleClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsConsoleWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonConsole);
}

void SettingsWidget::onWalletRepairClicked() {
    ui->stackedWidgetContainer->slideInWgt(settingsWalletRepairWidget, QSlideStackedWidget::LEFT2RIGHT);
    selectOption(ui->pushButtonRepair);
}


void SettingsWidget::onAboutClicked() {
    if (!clientModel)
        return;

    HelpMessageDialog dlg(this, true);
    dlg.exec();

}

void SettingsWidget::selectOption(QPushButton* option){
    for (QPushButton* wid : options) {
        if(wid) wid->setChecked(wid == option);
    }
}

void SettingsWidget::onDiscardChanges(){
    if(clientModel) {
        if (!ask(tr("Discard Unsaved Changes"), tr("You are just about to discard all of your unsaved options.\n\nAre you sure?\n")))
            return;
        clientModel->getOptionsModel()->refreshDataView();
    }
}

void SettingsWidget::setMapper(){
    settingsMainOptionsWidget->setMapper(mapper);
    settingsWalletOptionsWidget->setMapper(mapper);
    settingsDisplayOptionsWidget->setMapper(mapper);
}

bool SettingsWidget::openStandardDialog(QString title, QString body, QString okBtn, QString cancelBtn){
    showHideOp(true);
    DefaultDialog *confirmDialog = new DefaultDialog(window);
    confirmDialog->setText(title, body, okBtn, cancelBtn);
    confirmDialog->adjustSize();
    openDialogWithOpaqueBackground(confirmDialog, window);
    confirmDialog->deleteLater();
    return confirmDialog->isOk;
}

SettingsWidget::~SettingsWidget(){
    delete ui;
}
