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

#define DECORATION_SIZE 60
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
                new ContactsHolder(isLightTheme()),
                this
    );

    // List Addresses
    ui->listAddresses->setItemDelegate(delegate);
    ui->listAddresses->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listAddresses->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listAddresses->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listAddresses->setSelectionBehavior(QAbstractItemView::SelectRows);

    //Empty List
    ui->emptyContainer->setVisible(false);
    setCssProperty(ui->pushImgEmpty, "img-empty-contacts");

    ui->labelEmpty->setText(tr("No contacts yet"));
    setCssProperty(ui->labelEmpty, "text-empty");

    // Name
    ui->lineEditName->setPlaceholderText(tr("Contact name"));
    setCssProperty(ui->lineEditName, "edit-primary");
    //setCssEditLine(ui->lineEditName, true);

    // Address
    ui->lineEditAddress->setPlaceholderText("ALQO Address");
    //setCssEditLine(ui->lineEditAddress, true);
    setCssProperty(ui->lineEditAddress, "edit-primary");
    ui->lineEditAddress->setValidator(new QRegExpValidator(QRegExp("^[A-Za-z0-9]+"), ui->lineEditName));

    // Buttons
    ui->btnSave->setText(tr("SAVE"));
    setCssBtnPrimary(ui->btnSave);

    connect(ui->listAddresses, SIGNAL(clicked(QModelIndex)), this, SLOT(handleAddressClicked(QModelIndex)));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(onStoreContactClicked()));
}

void AddressesWidget::handleAddressClicked(const QModelIndex &index){
    ui->listAddresses->setCurrentIndex(index);
    QRect rect = ui->listAddresses->visualRect(index);
    QPoint pos = rect.topRight();
    pos.setX(pos.x() - (DECORATION_SIZE * 2));
    pos.setY(pos.y() + (DECORATION_SIZE));

    QModelIndex rIndex = filter->mapToSource(index);

    if(!this->menu){
        this->menu = new TooltipMenu(window, this);
        connect(this->menu, &TooltipMenu::message, this, &AddressesWidget::message);
        connect(this->menu, SIGNAL(onEditClicked()), this, SLOT(onEditClicked()));
        connect(this->menu, SIGNAL(onDeleteClicked()), this, SLOT(onDeleteClicked()));
        connect(this->menu, SIGNAL(onCopyClicked()), this, SLOT(onCopyClicked()));
    }else {
        this->menu->hide();
    }
    this->index = rIndex;
    menu->move(pos);
    menu->show();
}

void AddressesWidget::loadWalletModel(){
    if(walletModel) {
        addressTablemodel = walletModel->getAddressTableModel();
        this->filter = new AddressFilterProxyModel(AddressTableModel::Send, this);
        this->filter->setSourceModel(addressTablemodel);
        ui->listAddresses->setModel(this->filter);
        ui->listAddresses->setModelColumn(AddressTableModel::Address);

        updateListView();
    }
}

void AddressesWidget::updateListView(){
    bool empty = addressTablemodel->sizeSend() == 0;
    ui->emptyContainer->setVisible(empty);
    ui->listAddresses->setVisible(!empty);
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

        {
            ui->lineEditAddress->setText("");
            ui->lineEditName->setText("");
            setCssEditLine(ui->lineEditAddress, true, true);
            setCssEditLine(ui->lineEditName, true, true);

            if (ui->emptyContainer->isVisible()) {
                ui->emptyContainer->setVisible(false);
                ui->listAddresses->setVisible(true);
            }
            inform(tr("New Contact Stored"));
        }
    }
}

void AddressesWidget::onEditClicked(){
    QString address = index.data(Qt::DisplayRole).toString();
    QString currentLabel = index.sibling(index.row(), AddressTableModel::Label).data(Qt::DisplayRole).toString();
    showHideOp(true);
    AddNewContactDialog *dialog = new AddNewContactDialog(window);
    dialog->setData(address, currentLabel);
    if(openDialogWithOpaqueBackground(dialog, window)){
      if(walletModel->updateAddressBookLabels(CBitcoinAddress(address.toStdString()).Get(), dialog->getLabel().toStdString(), addressTablemodel->purposeForAddress(address.toStdString()))){
        inform(tr("Contact edited"));
      }else{
        inform(tr("Contact edit failed"));
      }
    }
}

void AddressesWidget::onDeleteClicked(){
    if(walletModel) {
            if (this->walletModel->getAddressTableModel()->removeRows(index.row(), 1, index)) {
                updateListView();
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

void AddressesWidget::onAddContactShowHideClicked(){

}

void AddressesWidget::changeTheme(bool isLightTheme, QString& theme){
    static_cast<ContactsHolder*>(this->delegate->getRowFactory())->isLightTheme = isLightTheme;
}

AddressesWidget::~AddressesWidget(){
    delete ui;
}
