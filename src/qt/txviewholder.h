// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TXVIEWHOLDER_H
#define TXVIEWHOLDER_H

#include "qt/furlistrow.h"
#include "qt/txrow.h"
#include "bitcoinunits.h"
#include <transactionfilterproxy.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class TxViewHolder : public FurListRow<QWidget*>
{
public:
    TxViewHolder();

    explicit TxViewHolder(bool _isLightTheme, bool _mini) : FurListRow(), isLightTheme(_isLightTheme), mini(_mini){}

    QWidget* createHolder(int pos) override;

    void init(QWidget* holder,const QModelIndex &index, bool isHovered, bool isSelected) const override;

    QColor rectColor(bool isHovered, bool isSelected) override;

    ~TxViewHolder() override{};

    bool isLightTheme;

    void setDisplayUnit(int displayUnit){
        this->nDisplayUnit = displayUnit;
    }

    void setFilter(TransactionFilterProxy *_filter){
        this->filter = _filter;
    }

private:
    int nDisplayUnit;
    TransactionFilterProxy *filter = nullptr;
    TxRow* txRow = nullptr;
    bool mini = false;
};

#endif // TXVIEWHOLDER_H
