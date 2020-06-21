// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/navmenuwidget.h>
#include <qt/forms/ui_navmenuwidget.h>
#include <qt/pivxgui.h>
#include <qt/qtutils.h>
#include <clientversion.h>
#include <qt/optionsmodel.h>
#include <qt/walletmodel.h>
#include <qt/clientmodel.h>
#include <QDebug>
#include <QDesktopServices>

NavMenuWidget::NavMenuWidget(ALQOGUI *mainWindow, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavMenuWidget),
    window(mainWindow)
{
    ui->setupUi(this);
    //this->setFixedWidth(100);
    setCssProperty(ui->navContainer_2, "container-nav");
    // Buttons
    logoButton = new QPushButton(this);
    logoButton->setGeometry(0, 0, 100, 100);
    logoButton->setIconSize(QSize(100, 100));
    setCssProperty(logoButton, "img-nav-logo");
    UpdateLogoButtonPos();

    ui->pushButtonConnection->setButtonClassStyle("cssClass", "btn-check-connect-inactive");
    ui->pushButtonConnection->setButtonText("No Connection");

    ui->pushButtonStack->setButtonClassStyle("cssClass", "btn-check-stack-inactive");
    ui->pushButtonStack->setButtonText("Staking Disabled");

    ui->pushButtonMint->setButtonClassStyle("cssClass", "btn-check-mint-inactive");
    ui->pushButtonMint->setButtonText("Automint Enabled");
    ui->pushButtonMint->setVisible(false);

    ui->pushButtonSync->setButtonClassStyle("cssClass", "btn-check-sync");
    ui->pushButtonSync->setButtonText(" %54 Synchronizing..");

    ui->pushButtonLock->setButtonText("Wallet Locked  ");
    ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-lock");

    connect(ui->pushButtonLock, SIGNAL(Mouse_Pressed()), this, SLOT(onBtnLockClicked()));
    connect(ui->btnDashboard,SIGNAL(clicked()),this, SLOT(onDashboardClicked()));
    connect(ui->btnSend,SIGNAL(clicked()),this, SLOT(onSendClicked()));
    connect(ui->btnAddress,SIGNAL(clicked()),this, SLOT(onAddressClicked()));
    connect(ui->btnMaster,SIGNAL(clicked()),this, SLOT(onMasterNodesClicked()));
    connect(ui->btnSettings,SIGNAL(clicked()),this, SLOT(onSettingsClicked()));
    connect(ui->btnReceive,SIGNAL(clicked()),this, SLOT(onReceiveClicked()));
    connect(ui->btnHistory,SIGNAL(clicked()),this, SLOT(onHistoryClicked()));
    connect(ui->btnCharts,SIGNAL(clicked()),this, SLOT(onChartsClicked()));
    connect(window, SIGNAL(windowResizeEvent(QResizeEvent*)), this, SLOT(windowResizeEvent(QResizeEvent*)));
    setCssProperty(ui->labelLogo, "label-logo");
    connect(logoButton, SIGNAL(clicked()), this, SLOT(slotOpenUrl()));
    
    connectActions();
}

void NavMenuWidget::UpdateLogoButtonPos() {
    logoButton->move((window->size().width() - logoButton->size().width()) / 2, 20);
}

void NavMenuWidget::windowResizeEvent(QResizeEvent* event) {
    UpdateLogoButtonPos();
    if (dlg) {
        dlg->move(QPoint((window->width() - dlg->width()) / 2, (window->height() - dlg->height()) / 2));
    }
}
void NavMenuWidget::slotOpenUrl() {
    QDesktopServices::openUrl(QUrl("https://alqo.app"));
}

/**
 * Actions
 */
void NavMenuWidget::connectActions() {

    ui->btnDashboard->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_1));
    ui->btnSend->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_2));
    ui->btnReceive->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_3));
    ui->btnHistory->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_4));
    ui->btnCharts->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_5));
    ui->btnAddress->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_6));
    ui->btnMaster->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_7));
    ui->btnSettings->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_8));
}

void NavMenuWidget::onSendClicked(){
    window->goToSend();
}

void NavMenuWidget::onDashboardClicked(){
    window->goToDashboard();
}

void NavMenuWidget::onChartsClicked(){
    window->goToCharts();
}

void NavMenuWidget::onHistoryClicked(){
    window->goToHistory();
}

void NavMenuWidget::onAddressClicked(){
    window->goToAddresses();
}

void NavMenuWidget::onMasterNodesClicked(){
    window->goToMasterNodes();
}

void NavMenuWidget::onSettingsClicked(){
    window->goToSettings();
}

void NavMenuWidget::onReceiveClicked(){
    window->goToReceive();
}

void NavMenuWidget::selectSettings(){
    onSettingsClicked();
}

void NavMenuWidget::onBtnLockClicked(){
    if(walletModel) {
        if (walletModel->getEncryptionStatus() == WalletModel::Unencrypted) {
            encryptWallet();
        } else {
            if (!lockUnlockWidget) {
                lockUnlockWidget = new LockUnlock(window);
                lockUnlockWidget->setStyleSheet("margin:0px; padding:0px;");
                connect(lockUnlockWidget, SIGNAL(Mouse_Leave()), this, SLOT(lockDropdownMouseLeave()));
                connect(ui->pushButtonLock, &ExpandableButton::Mouse_HoverLeave, [this](){
                    QMetaObject::invokeMethod(this, "lockDropdownMouseLeave", Qt::QueuedConnection);
                }); //, SLOT(lockDropdownMouseLeave()));
                connect(lockUnlockWidget, SIGNAL(lockClicked(const StateClicked&)),this, SLOT(lockDropdownClicked( const StateClicked&)));
            }

            lockUnlockWidget->updateStatus(walletModel->getEncryptionStatus());
            //if (ui->pushButtonLock->height() <= 40) {
                ui->pushButtonLock->setExpanded();
            //}
            // Keep it open
            ui->pushButtonLock->setKeepExpanded(true);
			//lockUnlockWidget->setFixedHeight(ui->pushButtonLock->height());

            QMetaObject::invokeMethod(this, "openLockUnlock", Qt::QueuedConnection);
        }
    }
}

void NavMenuWidget::openLockUnlock(){
    lockUnlockWidget->setFixedWidth(ui->pushButtonLock->width());
    lockUnlockWidget->adjustSize();

    lockUnlockWidget->move(window->getNavWidth() - ui->pushButtonLock->pos().x() - 8, ui->pushButtonLock->y() + ui->pushButtonLock->height() + 12);

    lockUnlockWidget->raise();
    lockUnlockWidget->activateWindow();
    lockUnlockWidget->show();
}

void NavMenuWidget::encryptWallet() {
    if (!walletModel)
        return;

    dlg = new AskPassphraseDialog(AskPassphraseDialog::Mode::Encrypt, window,
                            walletModel, AskPassphraseDialog::Context::Encrypt);
    dlg->adjustSize();
    window->showHide(true);
    openDialogWithOpaqueBackgroundY(dlg, window);

    refreshStatus();
    dlg->deleteLater();
}

static bool isExecuting = false;
void NavMenuWidget::lockDropdownClicked(const StateClicked& state){
    lockUnlockWidget->close();
    if(walletModel && !isExecuting) {
        isExecuting = true;

        switch (lockUnlockWidget->lock) {
            case 0: {
                if (walletModel->getEncryptionStatus() == WalletModel::Locked)
                    break;
                walletModel->setWalletLocked(true);
                ui->pushButtonLock->setButtonText("Wallet Locked");
                ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-lock", true);
                break;
            }
            case 1: {
                if (walletModel->getEncryptionStatus() == WalletModel::Unlocked)
                    break;
                AskPassphraseDialog *dlg = new AskPassphraseDialog(AskPassphraseDialog::Mode::Unlock, window, walletModel,
                                        AskPassphraseDialog::Context::ToggleLock);
                dlg->adjustSize();
                openDialogWithOpaqueBackgroundY(dlg, window);
                if (this->walletModel->getEncryptionStatus() == WalletModel::Unlocked) {
                    ui->pushButtonLock->setButtonText("Wallet Unlocked");
                    ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-unlock", true);
                }
                dlg->deleteLater();
                break;
            }
            case 2: {
                if (walletModel->getEncryptionStatus() == WalletModel::UnlockedForAnonymizationOnly)
                    break;
                AskPassphraseDialog *dlg = new AskPassphraseDialog(AskPassphraseDialog::Mode::UnlockAnonymize, window, walletModel,
                                        AskPassphraseDialog::Context::ToggleLock);
                dlg->adjustSize();
                openDialogWithOpaqueBackgroundY(dlg, window);
                if (this->walletModel->getEncryptionStatus() == WalletModel::UnlockedForAnonymizationOnly) {
                    ui->pushButtonLock->setButtonText(tr("Wallet Unlocked for staking"));
                    ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-staking", true);
                }
                dlg->deleteLater();
                break;
            }
        }

        ui->pushButtonLock->setKeepExpanded(false);
        ui->pushButtonLock->setSmall();
        ui->pushButtonLock->update();

        isExecuting = false;
    }
}

void NavMenuWidget::lockDropdownMouseLeave(){
    if (lockUnlockWidget->isVisible() && !lockUnlockWidget->isHovered()) {
        lockUnlockWidget->hide();
        ui->pushButtonLock->setKeepExpanded(false);
        ui->pushButtonLock->setSmall();
        ui->pushButtonLock->update();
    }
}

void NavMenuWidget::setClientModel(ClientModel* model){
	this->clientModel = model;
    if(clientModel){
        // Keep up to date with client
        setNumConnections(clientModel->getNumConnections());
        connect(clientModel, SIGNAL(numConnectionsChanged(int)), this, SLOT(setNumConnections(int)));

        setNumBlocks(clientModel->getNumBlocks());
        connect(clientModel, SIGNAL(numBlocksChanged(int)), this, SLOT(setNumBlocks(int)));

        timerStakingIcon = new QTimer(ui->pushButtonStack);
        connect(timerStakingIcon, SIGNAL(timeout()), this, SLOT(updateStakingStatus()));
        timerStakingIcon->start(50000);
        updateStakingStatus();
    }
}

void NavMenuWidget::setWalletModel(WalletModel* model){
	this->walletModel = model;
    connect(walletModel, &WalletModel::encryptionStatusChanged, this, &NavMenuWidget::refreshStatus);
    refreshStatus();
}

void NavMenuWidget::updateAutoMintStatus(){
    ui->pushButtonMint->setButtonText(fEnableZeromint ? tr("Automint enabled") : tr("Automint disabled"));
    ui->pushButtonMint->setChecked(fEnableZeromint);
}

void NavMenuWidget::updateStakingStatus(){
    if (nLastCoinStakeSearchInterval) {
        if (!ui->pushButtonStack->isChecked()) {
            ui->pushButtonStack->setButtonText(tr("Staking active"));
            ui->pushButtonStack->setChecked(true);
            ui->pushButtonStack->setButtonClassStyle("cssClass", "btn-check-stack", true);
        }
    }else{
        if (ui->pushButtonStack->isChecked()) {
            ui->pushButtonStack->setButtonText(tr("Staking not active"));
            ui->pushButtonStack->setChecked(false);
            ui->pushButtonStack->setButtonClassStyle("cssClass", "btn-check-stack-inactive", true);
        }
    }
}

void NavMenuWidget::setNumConnections(int count) {
    if(count > 0){
        if(!ui->pushButtonConnection->isChecked()) {
            ui->pushButtonConnection->setChecked(true);
            ui->pushButtonConnection->setButtonClassStyle("cssClass", "btn-check-connect", true);
        }
    }else{
        if(ui->pushButtonConnection->isChecked()) {
            ui->pushButtonConnection->setChecked(false);
            ui->pushButtonConnection->setButtonClassStyle("cssClass", "btn-check-connect-inactive", true);
        }
    }

    ui->pushButtonConnection->setButtonText(tr("%n active connection(s)", "", count));
}

void NavMenuWidget::setNumBlocks(int count) {
    if (!clientModel)
        return;

    // Acquire current block source
    enum BlockSource blockSource = clientModel->getBlockSource();
    std::string text = "";
    switch (blockSource) {
        case BLOCK_SOURCE_NETWORK:
            text = "Synchronizing..";
            break;
        case BLOCK_SOURCE_DISK:
            text = "Importing blocks from disk..";
            break;
        case BLOCK_SOURCE_REINDEX:
            text = "Reindexing blocks on disk..";
            break;
        case BLOCK_SOURCE_NONE:
            // Case: not Importing, not Reindexing and no network connection
            text = "No block source available..";
            ui->pushButtonSync->setChecked(false);
            break;
    }

    bool needState = true;
    if (masternodeSync.IsBlockchainSynced()) {
        // chain synced
        emit walletSynced(true);
        if (masternodeSync.IsSynced()) {
            // Node synced
            // TODO: Set synced icon to pushButtonSync here..
            ui->pushButtonSync->setButtonText(tr("Synchronized"));
            return;
        }else{

            // TODO: Show out of sync warning
            int nAttempt = masternodeSync.RequestedMasternodeAttempt < MASTERNODE_SYNC_THRESHOLD ?
                       masternodeSync.RequestedMasternodeAttempt + 1 :
                       MASTERNODE_SYNC_THRESHOLD;
            int progress = nAttempt + (masternodeSync.RequestedMasternodeAssets - 1) * MASTERNODE_SYNC_THRESHOLD;
            if(progress >= 0){
                // todo: MN progress..
                text = std::string("Synchronizing additional data..");//: %p%", progress);
                //progressBar->setMaximum(4 * MASTERNODE_SYNC_THRESHOLD);
                //progressBar->setValue(progress);
                needState = false;
            }
        }
    } else {
        emit walletSynced(false);
    }

    if(needState) {
        // Represent time from last generated block in human readable text
        QDateTime lastBlockDate = clientModel->getLastBlockDate();
        QDateTime currentDate = QDateTime::currentDateTime();
        int secs = lastBlockDate.secsTo(currentDate);

        QString timeBehindText;
        const int HOUR_IN_SECONDS = 60 * 60;
        const int DAY_IN_SECONDS = 24 * 60 * 60;
        const int WEEK_IN_SECONDS = 7 * 24 * 60 * 60;
        const int YEAR_IN_SECONDS = 31556952; // Average length of year in Gregorian calendar
        if (secs < 2 * DAY_IN_SECONDS) {
            timeBehindText = tr("%n hour(s)", "", secs / HOUR_IN_SECONDS);
        } else if (secs < 2 * WEEK_IN_SECONDS) {
            timeBehindText = tr("%n day(s)", "", secs / DAY_IN_SECONDS);
        } else if (secs < YEAR_IN_SECONDS) {
            timeBehindText = tr("%n week(s)", "", secs / WEEK_IN_SECONDS);
        } else {
            int years = secs / YEAR_IN_SECONDS;
            int remainder = secs % YEAR_IN_SECONDS;
            timeBehindText = tr("%1 and %2").arg(tr("%n year(s)", "", years)).arg(
                    tr("%n week(s)", "", remainder / WEEK_IN_SECONDS));
        }
        QString timeBehind(" behind. Scanning block ");
        QString str = timeBehindText + timeBehind + QString::number(count);
        text = str.toStdString();

    }

    if(text.empty()){
        text = "No block source available..";
    }

    ui->pushButtonSync->setButtonText(tr(text.data()));
}

void NavMenuWidget::refreshStatus(){
    // Check lock status
    if (!this->walletModel)
        return;

    WalletModel::EncryptionStatus encStatus = walletModel->getEncryptionStatus();

    switch (encStatus){
        case WalletModel::EncryptionStatus::Unencrypted:
            ui->pushButtonLock->setButtonText("Wallet Unencrypted");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-unlock", true);
            break;
        case WalletModel::EncryptionStatus::Locked:
            ui->pushButtonLock->setButtonText("Wallet Locked");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-lock", true);
            break;
        case WalletModel::EncryptionStatus::UnlockedForAnonymizationOnly:
            ui->pushButtonLock->setButtonText("Wallet Unlocked for staking");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-staking", true);
            break;
        case WalletModel::EncryptionStatus::Unlocked:
            ui->pushButtonLock->setButtonText("Wallet Unlocked");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-unlock", true);
            break;
    }
    updateStyle(ui->pushButtonLock);
}

void NavMenuWidget::resizeEvent(QResizeEvent *event){
    if (lockUnlockWidget && lockUnlockWidget->isVisible()) lockDropdownMouseLeave();
    QWidget::resizeEvent(event);
}

NavMenuWidget::~NavMenuWidget(){
    if(timerStakingIcon){
        timerStakingIcon->stop();
    }
    delete ui;
}
