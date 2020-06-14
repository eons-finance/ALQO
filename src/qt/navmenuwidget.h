// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NAVMENUWIDGET_H
#define NAVMENUWIDGET_H

#include <qt/pwidget.h>
#include <qt/lockunlock.h>
#include <QWidget>
#include <QPushButton>

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
    void UpdateLogoButtonPos();

protected:
    void resizeEvent(QResizeEvent *event) override;
    
public slots:
    void selectSettings();
    void setNumConnections(int count);
    void setNumBlocks(int count);
    void updateAutoMintStatus();
    void updateStakingStatus();
    void windowResizeEvent(QResizeEvent* event);

private slots:
    void onSendClicked();
    void onDashboardClicked();
    void onChartsClicked();
    void onHistoryClicked();
    void onAddressClicked();
    void onMasterNodesClicked();
    void onSettingsClicked();
    void onReceiveClicked();
    void onBtnLockClicked();
    void lockDropdownMouseLeave();
    void lockDropdownClicked(const StateClicked&);
    void refreshStatus();
    void openLockUnlock();
    void slotOpenUrl();

signals:
    void walletSynced(bool isSync);
    
private:
    Ui::NavMenuWidget *ui;
    ALQOGUI* window;
    AskPassphraseDialog *dlg = nullptr;
    LockUnlock *lockUnlockWidget = nullptr;
    QList<QWidget*> btns;
    QPushButton* logoButton;
    
    ClientModel* clientModel = nullptr;
    WalletModel* walletModel = nullptr;
    
    void connectActions();

    QTimer* timerStakingIcon = nullptr;
};

#endif // NAVMENUWIDGET_H
