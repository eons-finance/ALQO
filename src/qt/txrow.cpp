// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/txrow.h"
#include "qt/forms/ui_txrow.h"

#include "guiutil.h"
#include "qt/qtutils.h"

TxRow::TxRow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TxRow)
{
    ui->setupUi(this);
    //setCssProperty(ui->frame, "tx-frame");
}

void TxRow::init(bool isLightTheme) {
    setConfirmStatus(true);
    updateStatus(isLightTheme, false, false);
}

void TxRow::setConfirmStatus(bool isConfirm){
    if(isConfirm){
        setCssProperty(ui->lblAddress, "text-list-body1");
    }else{
        setCssProperty(ui->lblAddress, "text-list-body-unconfirmed");
    }
}

void TxRow::updateStatus(bool isLightTheme, bool isHover, bool isSelected){
    //if(isLightTheme)
    //    ui->lblDivisory->setStyleSheet("background-color:#1a1d31");
    //else
    //    ui->lblDivisory->setStyleSheet("background-color:#40ffff");
}

void TxRow::setDate(QDateTime date){
//    ui->lblDate->setText(GUIUtil::dateTimeStr(date));
}

void TxRow::setLabel(QString str){
    ui->lblAddress->setText(str);
    setCssProperty(ui->lblAddress, "tx-source");
}

void TxRow::setAmount(QString str){
    setCssProperty(ui->lblAmount, "tx-amount");
    ui->lblAmount->setText(str);
}

void TxRow::setType(bool isLightTheme, int type, bool isConfirmed){
    QString path;
    QString css;
    bool sameIcon = false;
    switch (type) {
        case TransactionRecord::ZerocoinMint:
            path = "://ic-transaction-mint";
            css = "text-list-amount-send";
            break;
        case TransactionRecord::Generated:
        case TransactionRecord::StakeZPIV:
        case TransactionRecord::MNReward:
        case TransactionRecord::StakeMint:
            path = "://ic-transaction-staked";
            css = "text-list-amount-receive";
            break;
        case TransactionRecord::RecvWithObfuscation:
        case TransactionRecord::RecvWithAddress:
        case TransactionRecord::RecvFromOther:
        case TransactionRecord::RecvFromZerocoinSpend:
            path = "://ic-transaction-received";
            css = "text-list-amount-receive";
            break;
        case TransactionRecord::SendToAddress:
        case TransactionRecord::SendToOther:
        case TransactionRecord::ZerocoinSpend:
        case TransactionRecord::ZerocoinSpend_Change_zPiv:
        case TransactionRecord::ZerocoinSpend_FromMe:
            path = "://ic-transaction-sent";
            css = "text-list-amount-send";
            break;
        case TransactionRecord::SendToSelf:
            path = "://ic-transaction-mint";
            css = "text-list-amount-send";
            break;
        default:
            path = "://ic-pending";
            sameIcon = true;
            css = "text-list-amount-unconfirmed";
            break;
    }

    if (!isLightTheme && !sameIcon){
        path += "-dark";
    }

    if (!isConfirmed){
        css = "text-list-amount-unconfirmed";
        path += "-inactive";
        setConfirmStatus(false);
    }else{
        setConfirmStatus(true);
    }
    setCssProperty(ui->lblAmount, css, true);
    //ui->icon->setIcon(QIcon(path));
}

TxRow::~TxRow(){
    delete ui;
}
