// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/addresseswidget.h"
#include "qt/forms/ui_addresseswidget.h"
#include "qt/addresslabelrow.h"
#include "qt/addnewaddressdialog.h"
#include "qt/tooltipmenu.h"

#include "qt/addnewcontactdialog.h"
#include "qt/pivxgui.h"
#include "guiutil.h"
#include "qt/qtutils.h"
#include "walletmodel.h"

#include <QModelIndex>
#include <QRegExpValidator>

#define DECORATION_SIZE 50
#define NUM_ITEMS 3

class ContactsHolder : public FurListRow<QWidget*>
{
public:
    ContactsHolder();

    explicit ContactsHolder(bool _isLightTheme) : FurListRow(), isLightTheme(_isLightTheme){}

    AddressLabelRow* createHolder(int pos) override{
        if (!cachedRow) cachedRow = new AddressLabelRow();
        cachedRow->init(isLightTheme, false);
        return cachedRow;
    }

    void init(QWidget* holder,const QModelIndex &index, bool isHovered, bool isSelected) const override{
        AddressLabelRow* row = static_cast<AddressLabelRow*>(holder);

        row->updateState(isLightTheme, isHovered, isSelected);

        QString address = index.data(Qt::DisplayRole).toString();
        QModelIndex sibling = index.sibling(index.row(), AddressTableModel::Label);
        QString label = sibling.data(Qt::DisplayRole).toString();

        row->updateView(address, label);
    }

    QColor rectColor(bool isHovered, bool isSelected) override{
        return getRowColor(isLightTheme, isHovered, isSelected);
    }

    ~ContactsHolder() override{}

    bool isLightTheme;
    AddressLabelRow* cachedRow = nullptr;
};


AddressesWidget::AddressesWidget(ALQOGUI* parent) :
    PWidget(parent),
    ui(new Ui::AddressesWidget)
{
    ui->setupUi(this);

    this->setStyleSheet(parent->styleSheet());

    delegate = new FurAbstractListItemDelegate(
                DECORATION_SIZE,
                DECORATION_SIZE,
                new ContactsHolder(isLightTheme()),
                this
    );

    // List Addresses
    setCssProperty(ui->listAddresses, "listTransactions");
    setCssProperty(ui->framecontacts, "dash-frame");
    setCssProperty(ui->emptyContainer, "dash-frame");

    ui->listAddresses->setItemDelegate(delegate);
    ui->listAddresses->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listAddresses->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listAddresses->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listAddresses->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Sort Controls
    setCssProperty(ui->comboBoxSort, "btn-contacts-combo");
    setCssProperty(ui->comboBoxSortOrder, "btn-contacts-combo");
    connect(ui->comboBoxSort, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddressesWidget::onSortChanged);

    connect(ui->comboBoxSortOrder, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddressesWidget::onSortOrderChanged);
    //fillAddressSortControls(lineEdit, lineEditOrder, ui->comboBoxSort, ui->comboBoxSortOrder);
    ui->comboBoxSort->setView(new QListView());

    ui->comboBoxSort->addItem(QObject::tr("By Label"), AddressTableModel::Label);
    ui->comboBoxSort->addItem(QObject::tr("By Address"), AddressTableModel::Address);
    ui->comboBoxSort->addItem(QObject::tr("By Date"), AddressTableModel::Date);
    ui->comboBoxSort->setCurrentIndex(0);

    ui->comboBoxSortOrder->setView(new QListView());
    ui->comboBoxSortOrder->addItem("Asc", Qt::AscendingOrder);
    ui->comboBoxSortOrder->addItem("Desc", Qt::DescendingOrder);
    ui->comboBoxSortOrder->setCurrentIndex(0);

    //Empty List
    ui->emptyContainer->setVisible(false);
    setCssProperty(ui->pushImgEmpty, "img-empty-contacts");

    ui->labelEmpty->setText(tr("No contacts yet"));
    setCssProperty(ui->labelEmpty, "text-empty");

    // Name
    ui->lineEditName->setPlaceholderText(tr("Contact name"));
    setCssProperty(ui->lineEditName, "edit-primary");
    setCssEditLine(ui->lineEditName, true);


    connect(ui->lineEditName, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditTextChanged(const QString&)));

    ui->lineEditName->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEditAddress->setAttribute(Qt::WA_MacShowFocusRect, 0);
    // Address
    ui->lineEditAddress->setPlaceholderText("ALQO Address");
    setCssEditLine(ui->lineEditAddress, true);
    setCssProperty(ui->lineEditAddress, "edit-primary");
    ui->lineEditAddress->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9]+"), ui->lineEditName));

    // Buttons
    ui->btnSave->setText(tr("SAVE"));
    setCssBtnPrimary(ui->btnSave);

    ui->btnCopy->setText(tr("Copy"));
    ui->btnCopy->setLayoutDirection(Qt::RightToLeft);
    setCssProperty(ui->btnCopy, "btn-secundary-copy");

    ui->btnDelete->setText(tr("Delete"));
    ui->btnDelete->setLayoutDirection(Qt::RightToLeft);
    setCssProperty(ui->btnDelete, "btn-secondary-delete");

    ui->btnEdit->setText(tr("Edit"));
    ui->btnEdit->setLayoutDirection(Qt::RightToLeft);
    setCssProperty(ui->btnEdit, "btn-secondary-edit");

    connect(ui->listAddresses, SIGNAL(clicked(QModelIndex)), this, SLOT(handleAddressClicked(QModelIndex)));

    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(onStoreContactClicked()));

    connect(ui->btnCopy, SIGNAL(clicked()), this, SLOT(onCopyClicked()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
    connect(ui->btnEdit, SIGNAL(clicked()), this, SLOT(onEditClicked()));

    ui->framemodify->setVisible(false);

    loadWalletModel();
}

void AddressesWidget::lineEditTextChanged(const QString& text)
{
    if (text.length() > 20)
	{
		ui->lineEditName->setText(text.left(text.length()-1));
        inform(tr("Contacts name cannot exceed 20 characters"));
	}
}

void AddressesWidget::handleAddressClicked(const QModelIndex &index){
    ui->listAddresses->setCurrentIndex(index);
    ui->framemodify->setVisible(true);
    QModelIndex rIndex = filter->mapToSource(index);
    this->index = rIndex;
}

void AddressesWidget::loadWalletModel(){
    if(walletModel) {
        addressTablemodel = walletModel->getAddressTableModel();
        this->filter = new AddressFilterProxyModel(AddressTableModel::Send, this);
        this->filter->setSourceModel(addressTablemodel);
        this->filter->sort(sortType, sortOrder);
        ui->listAddresses->setModel(this->filter);
        ui->listAddresses->setModelColumn(AddressTableModel::Address);

        updateListView();
      }else{
        inform(tr("load Contacts failed"));
      }
}

void AddressesWidget::updateListView(){
    bool empty = walletModel->getAddressTableModel()->sizeSend() == 0;
    ui->emptyContainer->setVisible(empty);
    ui->framecontacts->setVisible(!empty);
    ui->framemodify->setVisible(false);
}

void AddressesWidget::onStoreContactClicked(){
    if (walletModel) {
        QString label = ui->lineEditName->text();
        QString address = ui->lineEditAddress->text();

        if (!walletModel->validateAddress(address)) {
            setCssEditLine(ui->lineEditAddress, false, true);
            inform(tr("Invalid Contact Address"));
            return;
        }

        CBitcoinAddress pivAdd = CBitcoinAddress(address.toUtf8().constData());
        if (walletModel->isMine(pivAdd)) {
            setCssEditLine(ui->lineEditAddress, false, true);
            inform(tr("Cannot store your own address as contact"));
            return;
        }

        QString storedLabel = walletModel->getAddressTableModel()->labelForAddress(address);

        if(!storedLabel.isEmpty()){
            inform(tr("Address already stored, label: %1").arg("\'"+storedLabel+"\'"));
            return;
        }

        if (walletModel->updateAddressBookLabels(CBitcoinAddress(address.toStdString()).Get(), label.toStdString(), "send")) {
            ui->lineEditAddress->setText("");
            ui->lineEditName->setText("");
            setCssEditLine(ui->lineEditAddress, true, true);
            setCssEditLine(ui->lineEditName, true, true);

            if (ui->emptyContainer->isVisible()) {
                ui->emptyContainer->setVisible(false);
                ui->framecontacts->setVisible(true);
            }
            inform(tr("New Contact Stored"));
        } else {
			//QString result = walletModel->getAddressTableModel()->addRow(AddressTableModel::Send, label, address);
				inform("New Contact store failed");
        }
    }
    updateListView();
}

void AddressesWidget::onEditClicked(){
    QString address = index.data(Qt::DisplayRole).toString();
    QString currentLabel = index.sibling(index.row(), AddressTableModel::Label).data(Qt::DisplayRole).toString();
    showHideOp(true);
    AddNewContactDialog *dialog = new AddNewContactDialog(window);
    dialog->setData(address, currentLabel);
    if(openDialogWithOpaqueBackground(dialog, window)){
      if(walletModel->updateAddressBookLabels(CBitcoinAddress(address.toStdString()).Get(), dialog->getLabel().toStdString(), addressTablemodel->purposeForAddress("send"))){
        inform(tr("Contact edited"));
      }else{
        inform(tr("Contact edit failed"));
      }
    }
    dialog->deleteLater();
}

void AddressesWidget::onDeleteClicked(){
    if(walletModel) {
            if (this->walletModel->getAddressTableModel()->removeRows(index.row(), 1, index)) {
                QTimer::singleShot(100, this, [this]() {
                    updateListView();
                });
                inform(tr("Contact Deleted"));
            } else {
                inform(tr("Error deleting a contact"));
            }
    }
}

void AddressesWidget::onCopyClicked(){
    GUIUtil::setClipboard(index.data(Qt::DisplayRole).toString());
    inform(tr("Address copied"));
}

void AddressesWidget::onSortChanged(int idx)
{
    sortType = (AddressTableModel::ColumnIndex) ui->comboBoxSort->itemData(idx).toInt();
    sortAddresses();
}

void AddressesWidget::onSortOrderChanged(int idx)
{
    sortOrder = (Qt::SortOrder) ui->comboBoxSortOrder->itemData(idx).toInt();
    sortAddresses();
}

void AddressesWidget::sortAddresses()
{
    if (this->filter)
        this->filter->sort(sortType, sortOrder);
}

void AddressesWidget::changeTheme(bool isLightTheme, QString& theme){
    static_cast<ContactsHolder*>(this->delegate->getRowFactory())->isLightTheme = isLightTheme;
}

AddressesWidget::~AddressesWidget(){
    delete ui;
}
