// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include "qt/pwidget.h"
#include "qt/furabstractlistitemdelegate.h"
#include "qt/furlistrow.h"
#include "transactiontablemodel.h"
#include "qt/txviewholder.h"
#include "transactionfilterproxy.h"
#include "qt/contactsdropdown.h"
#include <atomic>
#include <cstdlib>
#include <QWidget>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>

#if defined(HAVE_CONFIG_H)
#include "config/alqo-config.h" /* for USE_QTCHARTS */
#endif

class ALQOGUI;
class WalletModel;
class SendCoinsRecipient;

namespace Ui {
class DashboardWidget;
}

class SortEdit : public QLineEdit{
    Q_OBJECT
public:
    explicit SortEdit(QWidget* parent = nullptr) : QLineEdit(parent){}

    inline void mousePressEvent(QMouseEvent *) override{
        emit Mouse_Pressed();
    }

    ~SortEdit() override{}

signals:
    void Mouse_Pressed();

};

enum SortTx {
    DATE_ASC = 0,
    DATE_DESC = 1,
    AMOUNT_ASC = 2,
    AMOUNT_DESC = 3
};

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class DashboardWidget : public PWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(ALQOGUI* _window);
    ~DashboardWidget();

    void loadWalletModel() override;
    void run(int type) override;
    void onError(QString error, int type) override;

public slots:
    void walletSynced(bool isSync);
    /**
     * Show incoming transaction notification for new transactions.
     * The new items are those between start and end inclusive, under the given parent item.
    */
    void processNewTransaction(const QModelIndex& parent, int start, int /*end*/);

    void updateBalances(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
                        const CAmount& zerocoinBalance, const CAmount& unconfirmedZerocoinBalance, const CAmount& immatureZerocoinBalance,
                        const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);
signals:
    /** Notify that a new transaction appeared */
    void incomingTransaction(const QString& date, int unit, const CAmount& amount, const QString& type, const QString& address);
   
protected:

private slots:
    void handleTransactionClicked(const QModelIndex &index);
    void changeTheme(bool isLightTheme, QString &theme) override;
    void onSortChanged(const QString&);
    void onSortTypeChanged(const QString& value);
    void updateDisplayUnit();
    void showList();
    void onTxArrived(const QString& hash, const bool& isCoinStake, const bool& isCSAnyType);
    void onBtnReceiveClicked();
    void showSend();
    void showReceive();
    void onSendClicked();
    void onContactsClicked();
    void loadContacts();
    void SetExchangeInfoTextLabels();
    
private:
    Ui::DashboardWidget *ui;
    FurAbstractListItemDelegate* txViewDelegate;
    TransactionFilterProxy* filter;
    TxViewHolder* txHolder;
    TransactionTableModel* txModel;
    int nDisplayUnit = -1;
    bool isSync = false;
    SendCoinsRecipient *info = nullptr;
    bool send(QList<SendCoinsRecipient> recipients);
    ContactsDropdown *menuContacts = nullptr;
    QAction *btnContact;
    QString sendRequest(QString url);
    int timerid;
    QTimer* timer;
    int64_t lastrefresh;
    CAmount curbal;
    QPushButton *buttons[3];
};

#endif // DASHBOARDWIDGET_H
