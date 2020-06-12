// Copyright (c) 2020 BlockMechanic
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/historywidget.h>
#include <qt/forms/ui_historywidget.h>
#include <qt/pivxgui.h>
#include <qt/qtutils.h>
#include "qt/txrow.h"
#include "guiutil.h"
#include "walletmodel.h"
#include "clientmodel.h"
#include "optionsmodel.h"
#include "qt/sendconfirmdialog.h"

#define DECORATION_SIZE 58
#define NUM_ITEMS 3

HistoryWidget::HistoryWidget(ALQOGUI *parent) :
    PWidget(parent),
    ui(new Ui::HistoryWidget)
{
    ui->setupUi(this);
    
    this->setStyleSheet(parent->styleSheet());

    QFont fonttitle = ui->labelTitle->font();
	fonttitle.setPointSize(48);
	ui->labelTitle->setFont(fonttitle);

    txHolder = new TxViewHolder(isLightTheme(), false);
    txViewDelegate = new FurAbstractListItemDelegate(
        DECORATION_SIZE,
        DECORATION_SIZE,
        txHolder,
        this
    );

    // Delay before filtering transactions in ms
    static const int input_filter_delay = 200;

    QTimer* amount_typing_delay = new QTimer(this);
    amount_typing_delay->setSingleShot(true);
    amount_typing_delay->setInterval(input_filter_delay);

    QTimer* prefix_typing_delay = new QTimer(this);
    prefix_typing_delay->setSingleShot(true);
    prefix_typing_delay->setInterval(input_filter_delay);

    
    // Sort Transactions
    SortEdit* lineEdit = new SortEdit(ui->comboBoxSort);
    initComboBox(ui->comboBoxSort, lineEdit);
    connect(lineEdit, &SortEdit::Mouse_Pressed, [this](){ui->comboBoxSort->showPopup();});
    ui->comboBoxSort->addItem("Date desc", SortTx::DATE_DESC);
    ui->comboBoxSort->addItem("Date asc", SortTx::DATE_ASC);
    ui->comboBoxSort->addItem("Amount desc", SortTx::AMOUNT_ASC);
    ui->comboBoxSort->addItem("Amount asc", SortTx::AMOUNT_DESC);

    // Sort type
    SortEdit* lineEditType = new SortEdit(ui->comboBoxSortType);
    initComboBox(ui->comboBoxSortType, lineEditType);

    QSettings settings;
    ui->comboBoxSortType->addItem(tr("All"), TransactionFilterProxy::ALL_TYPES);
    ui->comboBoxSortType->addItem(tr("Received"), TransactionFilterProxy::TYPE(TransactionRecord::RecvWithAddress) | TransactionFilterProxy::TYPE(TransactionRecord::RecvFromOther));
    ui->comboBoxSortType->addItem(tr("Sent"), TransactionFilterProxy::TYPE(TransactionRecord::SendToAddress) | TransactionFilterProxy::TYPE(TransactionRecord::SendToOther));
    ui->comboBoxSortType->addItem(tr("Mined"), TransactionFilterProxy::TYPE(TransactionRecord::Generated));
    ui->comboBoxSortType->addItem(tr("Minted"), TransactionFilterProxy::TYPE(TransactionRecord::StakeMint));
    ui->comboBoxSortType->addItem(tr("MN reward"), TransactionFilterProxy::TYPE(TransactionRecord::MNReward));
    ui->comboBoxSortType->addItem(tr("To yourself"), TransactionFilterProxy::TYPE(TransactionRecord::SendToSelf));
    ui->comboBoxSortType->setCurrentIndex(0);

    // Transactions
    //setCssProperty(ui->frametransactions, "dash-frame");
    setCssProperty(ui->listTransactions, "listTransactions");

    ui->listTransactions->setItemDelegate(txViewDelegate);
    ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listTransactions->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listTransactions->setSelectionBehavior(QAbstractItemView::SelectRows);

    //Empty List
    ui->emptyContainer->setVisible(false);
    setCssProperty(ui->pushImgEmpty, "img-empty-transactions");

    ui->labelEmpty->setText(tr("No transactions yet"));
    setCssProperty(ui->labelEmpty, "text-empty");

    /* Filter */
    ui->lineEditFilter->setPlaceholderText("Filter by name/type/date");
    setCssProperty(ui->lineEditFilter, "edit-primary");

    // Button Search
    btnSearch = ui->lineEditFilter->addAction(QIcon("://ic-watch-password-white"), QLineEdit::TrailingPosition);

    connect(ui->lineEditAmount, &QLineEdit::textChanged, amount_typing_delay, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(amount_typing_delay, &QTimer::timeout, this, &HistoryWidget::changedAmount);
    connect(ui->lineEditFilter, &QLineEdit::textChanged, prefix_typing_delay, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(prefix_typing_delay, &QTimer::timeout, this, &HistoryWidget::changedSearch);
    connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));
    connect(ui->comboBoxSortType, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSortTypeChanged(const QString&)));
    connect(ui->comboBoxSort, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSortChanged(const QString&)));
    connect(lineEditType, &SortEdit::Mouse_Pressed, [this](){ui->comboBoxSortType->showPopup();});

    ui->lineEditAmount->setVisible(false);
    
    loadWalletModel();
}

void HistoryWidget::changedSearch()
{
    if(!filter)
        return;
    filter->setSearchString(ui->lineEditFilter->text());
}

void HistoryWidget::changedAmount()
{
    if(!filter)
        return;
    CAmount amount_parsed = 0;
    if (BitcoinUnits::parse(walletModel->getOptionsModel()->getDisplayUnit(), ui->lineEditAmount->text(), &amount_parsed)) {
        filter->setMinAmount(amount_parsed);
    }
    else
    {
        filter->setMinAmount(0);
    }
}

void HistoryWidget::loadWalletModel(){
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
            ui->emptyContainer->setVisible(true);
            ui->listTransactions->setVisible(false);
            ui->comboBoxSortType->setVisible(false);
            ui->comboBoxSort->setVisible(false);
        }

        connect(ui->pushImgEmpty, SIGNAL(clicked()), window, SLOT(openFAQ()));
        connect(ui->btnHowTo, SIGNAL(clicked()), window, SLOT(openFAQ()));

    }
    // update the display unit, to not use the default ("ALQO")
    updateDisplayUnit();
}

void HistoryWidget::handleTransactionClicked(const QModelIndex &index){

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

void HistoryWidget::showList(){
    if (filter->rowCount() == 0){
        ui->emptyContainer->setVisible(true);
        ui->listTransactions->setVisible(false);
        ui->comboBoxSortType->setVisible(false);
        ui->comboBoxSort->setVisible(false);
    } else {
        ui->emptyContainer->setVisible(false);
        ui->listTransactions->setVisible(true);
        ui->comboBoxSortType->setVisible(true);
        ui->comboBoxSort->setVisible(true);
    }
}

void HistoryWidget::updateDisplayUnit() {
    if (walletModel && walletModel->getOptionsModel()) {
        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
        txHolder->setDisplayUnit(nDisplayUnit);
        ui->listTransactions->update();
    }
}

void HistoryWidget::onSortChanged(const QString& value){
    if (!filter) return;
    int columnIndex = 0;
    Qt::SortOrder order = Qt::DescendingOrder;
    if(!value.isNull()) {
        switch (ui->comboBoxSort->itemData(ui->comboBoxSort->currentIndex()).toInt()) {
            case SortTx::DATE_ASC:{
                columnIndex = TransactionTableModel::Date;
                order = Qt::AscendingOrder;
                break;
            }
            case SortTx::DATE_DESC:{
                columnIndex = TransactionTableModel::Date;
                break;
            }
            case SortTx::AMOUNT_ASC:{
                columnIndex = TransactionTableModel::Amount;
                order = Qt::AscendingOrder;
                break;
            }
            case SortTx::AMOUNT_DESC:{
                columnIndex = TransactionTableModel::Amount;
                break;
            }

        }
    }
    filter->sort(columnIndex, order);
    ui->listTransactions->update();
}

void HistoryWidget::onSortTypeChanged(const QString& value){
    if (!filter) return;
    int filterByType = ui->comboBoxSortType->itemData(ui->comboBoxSortType->currentIndex()).toInt();
    filter->setTypeFilter(filterByType);
    ui->listTransactions->update();

    if (filter->rowCount() == 0){
        ui->emptyContainer->setVisible(true);
        ui->listTransactions->setVisible(false);
    } else {
        showList();
    }

    // Store settings
    QSettings settings;
    settings.setValue("transactionType", filterByType);
}

void HistoryWidget::onTxArrived(const QString& hash, const bool& isCoinStake, const bool& isCSAnyType) {
    showList();

}

HistoryWidget::~HistoryWidget(){
    delete ui;
}
