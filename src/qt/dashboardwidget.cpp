// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/dashboardwidget.h"
#include "qt/forms/ui_dashboardwidget.h"
#include "qt/sendconfirmdialog.h"
#include <qt/receivedialog.h>
#include "qt/txrow.h"
#include "qt/qtutils.h"
#include "guiutil.h"
#include "walletmodel.h"
#include "clientmodel.h"
#include "optionsmodel.h"
#include "utiltime.h"
#include <QPainter>
#include <QModelIndex>
#include <QList>
#include <QGraphicsLayout>

#define DECORATION_SIZE 40
#define NUM_ITEMS 3
#define SHOW_EMPTY_CHART_VIEW_THRESHOLD 4000
#define REQUEST_LOAD_TASK 1
#define CHART_LOAD_MIN_TIME_INTERVAL 15

DashboardWidget::DashboardWidget(ALQOGUI* parent) :
    PWidget(parent),
    ui(new Ui::DashboardWidget)
{
    ui->setupUi(this);

    txHolder = new TxViewHolder(isLightTheme());
    txViewDelegate = new FurAbstractListItemDelegate(
        DECORATION_SIZE,
        txHolder,
        this
    );
    
    this->setStyleSheet(parent->styleSheet());    
    this->setContentsMargins(0,0,0,0);
    ui->dashLayout->setContentsMargins(0,0,0,0);

    // Balance
    //setCssProperty(ui->labelAmountPiv, "dash-balance");
    QString totalPiv = GUIUtil::formatBalance(0, nDisplayUnit);

    QFont fontBalance = ui->labelAmountPiv->font();
    fontBalance.setWeight(QFont::Bold);
	fontBalance.setPointSize(48);
	ui->labelAmountPiv->setFont(fontBalance);
    ui->labelAmountPiv->setText("<font color='white'>"+totalPiv+"</font>");
	
    //Frames
    // Send Receive Frame
    //Send
    setCssProperty(ui->frameSendReceive, "dash-frame");
    setCssProperty(ui->sendpagebtn, "dash-label");
    setCssProperty(ui->receivepagebtn, "dash-label");
    setCssProperty(ui->lblRecentcontacts, "dash-label-sm");
    setCssProperty(ui->sendbtn, "dash-btn");

    //Receive
    setCssProperty(ui->pushButtonQR, "dash-btn");
    setCssProperty(ui->labelAddress, "dash-label-sm");

    // QR image
    QPixmap pixmap("://img-qr-test");
    ui->btnQr->setIcon(QIcon(pixmap.scaled(ui->btnQr->width(), ui->btnQr->height(), Qt::KeepAspectRatio)));


    // Transactions
    setCssProperty(ui->frameTransactions, "dash-frame");
    setCssProperty(ui->listTransactions, "listTransactions");
    setCssProperty(ui->lblTransactions, "dash-label");
    ui->listTransactions->setItemDelegate(txViewDelegate);
    ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listTransactions->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listTransactions->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Staking Info
    setCssProperty(ui->frameStaking, "dash-frame");
    setCssProperty(ui->lblStaking, "dash-label");
    setCssProperty(ui->lblAmountStaked, "dash-label-md");
    setCssProperty(ui->AmountStaked, "dash-label-md");
    setCssProperty(ui->lblRewards, "dash-label-md");
    setCssProperty(ui->Rewards, "dash-label");

    connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));
    if (window)
        connect(window, SIGNAL(windowResizeEvent(QResizeEvent*)), this, SLOT(windowResizeEvent(QResizeEvent*)));

    connect(ui->btnQr, SIGNAL(clicked()), this, SLOT(onBtnReceiveClicked()));
    connect(ui->pushButtonQR, SIGNAL(clicked()), this, SLOT(onBtnReceiveClicked()));
    
    connect(ui->sendpagebtn, SIGNAL(clicked()), this, SLOT(showSend()));
    connect(ui->receivepagebtn, SIGNAL(clicked()), this, SLOT(showReceive()));

    loadWalletModel();
}

void DashboardWidget::onBtnReceiveClicked(){
    if(walletModel) {
        showHideOp(true);
        ReceiveDialog *receiveDialog = new ReceiveDialog(window);

        receiveDialog->updateQr(walletModel->getAddressTableModel()->getLastUnusedAddress());
        if (openDialogWithOpaqueBackground(receiveDialog, window)) {
            inform(tr("Address Copied"));
        }
        receiveDialog->deleteLater();
    }
}

void DashboardWidget::handleTransactionClicked(const QModelIndex &index){

    ui->listTransactions->setCurrentIndex(index);
    QModelIndex rIndex = filter->mapToSource(index);

    window->showHide(true);
    TxDetailDialog *dialog = new TxDetailDialog(window, false);
    dialog->setData(walletModel, rIndex);
    openDialogWithOpaqueBackgroundY(dialog, window, 3, 17);

    // Back to regular status
    ui->listTransactions->scrollTo(index);
    ui->listTransactions->clearSelection();
    ui->listTransactions->setFocus();
    dialog->deleteLater();
}

void DashboardWidget::loadWalletModel(){
    if (walletModel && walletModel->getOptionsModel()) {
        txModel = walletModel->getTransactionTableModel();
        // Set up transaction list
        filter = new TransactionFilterProxy();
        filter->setDynamicSortFilter(true);
        filter->setSortCaseSensitivity(Qt::CaseInsensitive);
        filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
        filter->setSortRole(Qt::EditRole);
        filter->setSourceModel(txModel);
        filter->sort(TransactionTableModel::Date, Qt::DescendingOrder);
        txHolder->setFilter(filter);
        ui->listTransactions->setModel(filter);
        ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

        if(txModel->size() == 0){
            ui->listTransactions->setVisible(false);
        }

        //connect(txModel, &TransactionTableModel::txArrived, this, &DashboardWidget::onTxArrived);

        // Notification pop-up for new transaction
        connect(txModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(processNewTransaction(QModelIndex, int, int)));
        
        QString qraddress = walletModel->getAddressTableModel()->getLastUnusedAddress();
        ui->labelAddress->setText(qraddress);

		info = new SendCoinsRecipient();
		info->address = qraddress;
		QString uri = GUIUtil::formatBitcoinURI(*info);
		QString error;
		QPixmap pixmap = encodeToQr(uri, error);
		if(!pixmap.isNull()){
			ui->btnQr->setIcon(QIcon(pixmap.scaled(ui->btnQr->width(), ui->btnQr->height(), Qt::KeepAspectRatio)));
		}else{
			ui->btnQr->setText(!error.isEmpty() ? error : "Error encoding address");
		}
    }
    // update the display unit, to not use the default ("ALQO")    
    updateDisplayUnit();
}

void DashboardWidget::onTxArrived(const QString& hash, const bool& isCoinStake, const bool& isCSAnyType) {
    showList();
}

void DashboardWidget::showList(){
    if (filter->rowCount() == 0){
  //      ui->listTransactions->setVisible(false);
    } else {
  //      ui->listTransactions->setVisible(true);
    }
}

void DashboardWidget::updateBalances(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
                            const CAmount& zerocoinBalance, const CAmount& unconfirmedZerocoinBalance, const CAmount& immatureZerocoinBalance,
                            const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance){

    CAmount nLockedBalance = 0;
    if (!walletModel) {
        nLockedBalance = walletModel->getLockedBalance();
    }

    // ALQO Balance
    //CAmount nTotalBalance = balance + unconfirmedBalance;
    CAmount pivAvailableBalance = balance - immatureBalance - nLockedBalance;
    // Set
    //QString totalPiv = QString::number(pivAvailableBalance);
    QString totalPiv = BitcoinUnits::simplestFormat(nDisplayUnit, pivAvailableBalance, 2, false, BitcoinUnits::separatorAlways);

    QFont fontBalance = ui->labelAmountPiv->font();
    fontBalance.setWeight(QFont::Bold);
	fontBalance.setPointSize(48);
	ui->labelAmountPiv->setFont(fontBalance);
    ui->labelAmountPiv->setText("<font color='white'>"+totalPiv+"</font>");

}

void DashboardWidget::updateDisplayUnit() {
    if (walletModel && walletModel->getOptionsModel()) {
        int displayUnitPrev = nDisplayUnit;
        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
        txHolder->setDisplayUnit(nDisplayUnit);
        ui->listTransactions->update();

        if (displayUnitPrev != nDisplayUnit)
            updateBalances(walletModel->getBalance(), walletModel->getUnconfirmedBalance(), walletModel->getImmatureBalance(),
                           walletModel->getZerocoinBalance(), walletModel->getUnconfirmedZerocoinBalance(), walletModel->getImmatureZerocoinBalance(),
                           walletModel->getWatchBalance(), 0, 0);
        
    }
}

void DashboardWidget::onSortChanged(const QString& value){
    if (!filter) return;
    int columnIndex = 0;
    Qt::SortOrder order = Qt::DescendingOrder;
    if(!value.isNull()) {
    }
    filter->sort(columnIndex, order);
    ui->listTransactions->update();
}

void DashboardWidget::onSortTypeChanged(const QString& value){
    if (!filter) return;

}

void DashboardWidget::walletSynced(bool sync){
    if (this->isSync != sync) {
        this->isSync = sync;
        ui->layoutWarning->setVisible(!this->isSync);
    }
}

void DashboardWidget::changeTheme(bool isLightTheme, QString& theme){
    static_cast<TxViewHolder*>(this->txViewDelegate->getRowFactory())->isLightTheme = isLightTheme;
}

void DashboardWidget::run(int type) {
}
void DashboardWidget::onError(QString error, int type) {
    inform(tr("Error loading chart: %1").arg(error));
}

void DashboardWidget::processNewTransaction(const QModelIndex& parent, int start, int /*end*/) {
    // Prevent notifications-spam when initial block download is in progress
    if (!walletModel || !clientModel || clientModel->inInitialBlockDownload())
        return;

    if (!txModel || txModel->processingQueuedTransactions())
        return;

    QString date = txModel->index(start, TransactionTableModel::Date, parent).data().toString();
    qint64 amount = txModel->index(start, TransactionTableModel::Amount, parent).data(Qt::EditRole).toULongLong();
    QString type = txModel->index(start, TransactionTableModel::Type, parent).data().toString();
    QString address = txModel->index(start, TransactionTableModel::ToAddress, parent).data().toString();

    emit incomingTransaction(date, walletModel->getOptionsModel()->getDisplayUnit(), amount, type, address);
}

void DashboardWidget::showSend(){
    if(ui->stackedWidget->currentIndex() != 0){
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void DashboardWidget::showReceive(){
    if(ui->stackedWidget->currentIndex() != 1){
        ui->stackedWidget->setCurrentIndex(1);
    }
}

DashboardWidget::~DashboardWidget(){
    delete ui;
}
