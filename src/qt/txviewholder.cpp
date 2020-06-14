// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/txviewholder.h"
#include "qt/qtutils.h"
#include "transactiontablemodel.h"
#include <QModelIndex>

#define ADDRESSLEFT_SIZE 10
#define ADDRESSRIGHT_SIZE 5

QWidget* TxViewHolder::createHolder(int pos){
    if (!txRow) txRow = new TxRow();
    txRow->init(isLightTheme, mini);
    return txRow;
}

void TxViewHolder::init(QWidget* holder,const QModelIndex &index, bool isHovered, bool isSelected) const{
    TxRow *txRow = static_cast<TxRow*>(holder);
    txRow->updateStatus(isLightTheme, isHovered, isSelected);

    QModelIndex rIndex = (filter) ? filter->mapToSource(index) : index;
    QDateTime date = rIndex.data(TransactionTableModel::DateRole).toDateTime();
    qint64 amount = rIndex.data(TransactionTableModel::AmountRole).toLongLong();
    QString amountText = BitcoinUnits::simpleFormat(nDisplayUnit, amount, true, BitcoinUnits::separatorAlways);
    //QModelIndex indexType = rIndex.sibling(rIndex.row(),TransactionTableModel::Type);
    QString label ;//= indexType.data(Qt::DisplayRole).toString();
    int type = rIndex.data(TransactionTableModel::TypeRole).toInt();

    if(type != TransactionRecord::ZerocoinMint &&
            type !=  TransactionRecord::ZerocoinSpend_Change_zPiv &&
            type !=  TransactionRecord::StakeZPIV &&
            type != TransactionRecord::Other){
        QString address = rIndex.data(Qt::DisplayRole).toString();
        if(address.length() > 20) {
            address = address.left(ADDRESSLEFT_SIZE) + "..." + address.right(ADDRESSRIGHT_SIZE);
        }
        label += address;
    } else if (type == TransactionRecord::Other) {
        label += rIndex.data(Qt::DisplayRole).toString();
    }

    int status = rIndex.data(TransactionTableModel::StatusRole).toInt();
    bool isUnconfirmed = (status == TransactionStatus::Unconfirmed) || (status == TransactionStatus::Immature)
                         || (status == TransactionStatus::Conflicted) || (status == TransactionStatus::NotAccepted);

    txRow->setDate(date);
    txRow->setLabel(label);
    txRow->setAmount(amountText);
    txRow->setType(isLightTheme, type, !isUnconfirmed);
}

QColor TxViewHolder::rectColor(bool isHovered, bool isSelected) {
    return getRowColor(isLightTheme, isHovered, isSelected);
}
