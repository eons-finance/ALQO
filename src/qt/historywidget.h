// Copyright (c) 2020 BlockMechanic
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include "qt/pwidget.h"
#include "qt/furabstractlistitemdelegate.h"
#include "qt/furlistrow.h"
#include "transactiontablemodel.h"
#include "qt/txviewholder.h"
#include "transactionfilterproxy.h"

#include <QWidget>

class ALQOGUI;
class WalletModel;

namespace Ui {
class HistoryWidget;
}

class HistoryWidget : public PWidget
{
    Q_OBJECT

public:
    explicit HistoryWidget(ALQOGUI* parent);
    ~HistoryWidget();

    void loadWalletModel() override;


public slots:

    void changedAmount();
    void changedSearch();
   
private slots:

    void handleTransactionClicked(const QModelIndex &index);
    void onSortChanged(const QString&);
    void onSortTypeChanged(const QString& value);
    void updateDisplayUnit();
    void showList();
    void onTxArrived(const QString& hash, const bool& isCoinStake, const bool& isCSAnyType);
    
private:
    Ui::HistoryWidget *ui;
    FurAbstractListItemDelegate* txViewDelegate;
    TransactionFilterProxy* filter;
    TxViewHolder* txHolder;
    TransactionTableModel* txModel;
    int nDisplayUnit = -1;
    bool isSync = false;
    QAction *btnSearch;

signals:
    /** Notify that a new transaction appeared */
    void incomingTransaction(const QString& date, int unit, const CAmount& amount, const QString& type, const QString& address);
};

#endif // HISTORYWIDGET_H

