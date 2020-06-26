// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ADDRESSESWIDGET_H
#define ADDRESSESWIDGET_H

#include "qt/pwidget.h"
#include "addresstablemodel.h"
#include "furabstractlistitemdelegate.h"
#include "qt/addressfilterproxymodel.h"

#include <QWidget>

class AddressViewDelegate;
//class TooltipMenu;
class ALQOGUI;
class WalletModel;

namespace Ui {
class AddressesWidget;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class AddressesWidget : public PWidget
{
    Q_OBJECT

public:
    explicit AddressesWidget(ALQOGUI* parent);
    ~AddressesWidget();

    void loadWalletModel() override;
    void onNewContactClicked();
    void updateListView();

private slots:
    void handleAddressClicked(const QModelIndex &index);
    void onStoreContactClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onCopyClicked();
    void onSortChanged(int idx);
    void onSortOrderChanged(int idx);
    void lineEditTextChanged(const QString& text);

    void changeTheme(bool isLightTheme, QString &theme) override;
private:
    Ui::AddressesWidget *ui;

    FurAbstractListItemDelegate* delegate = nullptr;
    AddressTableModel* addressTablemodel = nullptr;
    AddressFilterProxyModel *filter = nullptr;

    bool isOnMyAddresses = true;
    //TooltipMenu* menu = nullptr;

    // Cached index
    QModelIndex index;

    // Cached sort type and order
    AddressTableModel::ColumnIndex sortType = AddressTableModel::Label;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;

    void sortAddresses();
};

#endif // ADDRESSESWIDGET_H
