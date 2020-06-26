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

void TxRow::init(bool isLightTheme, bool _mini) {
	this->mini=_mini;
    setConfirmStatus(true);
    updateStatus(isLightTheme, false, false);
    if (_mini) {
        ui->addressWidget->setMaximumWidth(160);
        ui->addressWidget->setMinimumWidth(160);
    } else {
        ui->addressWidget->setMaximumWidth(250);
        ui->addressWidget->setMinimumWidth(250);
    }
}

void TxRow::setConfirmStatus(bool isConfirm){
    if(isConfirm){
        setCssProperty(ui->lblAddress, "text-list-body1");
        setCssProperty(ui->lblDate, "text-list-caption");
    }else{
        setCssProperty(ui->lblAddress, "text-list-body-unconfirmed");
        setCssProperty(ui->lblDate,"text-list-caption-unconfirmed");
    }
}

void TxRow::updateStatus(bool isLightTheme, bool isHover, bool isSelected){
    //if(isLightTheme)
    //    ui->lblDivisory->setStyleSheet("background-color:#1a1d31");
    //else
    //    ui->lblDivisory->setStyleSheet("background-color:#40ffff");
}

void TxRow::setDate(QDateTime date){
    ui->lblDate->setText(GUIUtil::dateTimeStr(date));
}

void TxRow::setLabel(QString str){
    QString add = str;
    if (mini && add.length() > 23)
        add = add.left(15) + "..." + add.right(5);
    else if(add.length() > 40) {
        add = add.left(30) + "..." + add.right(10);
    }
    ui->lblAddress->setText(add);
}

void TxRow::setAmount(QString str){
    //QString amount = QString::number(str.toDouble(), 'f', 2);
    ui->lblAmount->setText(str);
}

void TxRow::setType(bool isLightTheme, int type, bool isConfirmed){
    QString path;
    QString css;
    QString txtype;
    QString color;

    switch (type) {
        case TransactionRecord::ZerocoinMint:
            path = "://ic-transaction-mint";
            css = "text-list-amount-send";
            txtype = "ZerocoinMint";
            color = "#3DE4CD";
            break;
        case TransactionRecord::Generated:
            path = "://ic-transaction-staked";
            css = "text-list-amount-receive";
            txtype = "Mined";
            color = "#3DE4CD";
            break;
        case TransactionRecord::MNReward:
            path = "://ic-transaction-staked";
            css = "text-list-amount-receive";
            txtype = "Masternode";
            color = "#3DE4CD";
            break;
        case TransactionRecord::StakeZPIV:
        case TransactionRecord::StakeMint:
            path = "://ic-transaction-staked";
            css = "text-list-amount-receive";
            txtype = "Stake";
            color = "#3DE4CD";
            break;
        case TransactionRecord::RecvWithObfuscation:
        case TransactionRecord::RecvWithAddress:
        case TransactionRecord::RecvFromOther:
        case TransactionRecord::RecvFromZerocoinSpend:
            path = "://ic-transaction-received";
            css = "text-list-amount-receive";
            txtype = "Received";
            color = "#3DE4CD";
            break;
        case TransactionRecord::SendToAddress:
        case TransactionRecord::SendToOther:
        case TransactionRecord::ZerocoinSpend:
        case TransactionRecord::ZerocoinSpend_Change_zPiv:
        case TransactionRecord::ZerocoinSpend_FromMe:
            path = "://ic-transaction-sent";
            css = "text-list-amount-send";
            txtype = "Sent";
            color = "#ff0000";
            break;
        case TransactionRecord::SendToSelf:
            path = "://ic-transaction-mint";
            css = "text-list-amount-send";
            txtype = "Moved";
            color = "#3DE4CD";
            break;
        default:
            path = "://ic-pending";
            css = "text-list-amount-unconfirmed";
            txtype = "Unknown";
            color = "#505BB9";
            break;
    }

    if (!isConfirmed){
        css = "text-list-amount-unconfirmed";
        path += "-inactive";
        setConfirmStatus(false);
    }else{
        setConfirmStatus(true);
    }

    if(!mini){
		ui->lblType->setText(txtype);		
		ui->lblType->setStyleSheet("color:"+color+"; text-align:center; font-weight: bold;");
	}else{		
        ui->widget->setVisible(false);
	}
    setCssProperty(ui->lblAmount, css, true);
    
}

void TxRow::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

TxRow::~TxRow(){
    delete ui;
}
