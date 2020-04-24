// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NAVMENUWIDGET_H
#define NAVMENUWIDGET_H

#include <qt/pwidget.h>
#include <qt/lockunlock.h>
#include <QWidget>

class ALQOGUI;
class WalletModel;
class ClientModel;

namespace Ui {
class NavMenuWidget;
}

class NavMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavMenuWidget(ALQOGUI* mainWindow, QWidget *parent = nullptr);
    ~NavMenuWidget();

    void setWalletModel(WalletModel* model);
    void setClientModel(ClientModel* model);
    void encryptWallet();

protected:
    void resizeEvent(QResizeEvent *event) override;
    
public slots:
    void selectSettings();
    void setNumConnections(int count);
    void setNumBlocks(int count);
    void updateAutoMintStatus();
    void updateStakingStatus();
    
private slots:
    void onSendClicked();
    void onDashboardClicked();
    void onChartsClicked();
    void onHistoryClicked();
    void onAddressClicked();
    void onMasterNodesClicked();
    void onSettingsClicked();
    void onReceiveClicked();
    void updateButtonStyles();
    void onBtnLockClicked();
    void lockDropdownMouseLeave();
    void lockDropdownClicked(const StateClicked&);
    void refreshStatus();
    void openLockUnlock();

signals:
    void walletSynced(bool isSync);
    
private:
    Ui::NavMenuWidget *ui;
    ALQOGUI* window;
    LockUnlock *lockUnlockWidget = nullptr;
    QList<QWidget*> btns;
    
    ClientModel* clientModel = nullptr;
    WalletModel* walletModel = nullptr;
    
    void connectActions();
    void onNavSelected(QWidget* active, bool startup = false);

    QTimer* timerStakingIcon = nullptr;
};

#endif // NAVMENUWIDGET_H
