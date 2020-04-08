// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/navmenuwidget.h>
#include <qt/forms/ui_navmenuwidget.h>
#include <qt/pivxgui.h>
#include <qt/qtutils.h>
#include <clientversion.h>

NavMenuWidget::NavMenuWidget(ALQOGUI *mainWindow, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavMenuWidget),
    window(mainWindow)
{
    ui->setupUi(this);
    //this->setFixedWidth(100);
    setCssProperty(ui->navContainer_2, "container-nav");
    setCssProperty(ui->imgLogo, "img-nav-logo");

    // Buttons
    ui->btnDashboard->setProperty("name", "dash");
    ui->btnDashboard->setText("HOME");
    ui->btnDashboard->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    ui->btnSend->setProperty("name", "send");
    ui->btnSend->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->btnSend->setText("SEND");

    ui->btnAddress->setProperty("name", "address");
    ui->btnAddress->setText("CONTACTS");
    ui->btnAddress->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    ui->btnHistory->setProperty("name", "history");
    ui->btnHistory->setText("HISTORY");
    ui->btnHistory->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    ui->btnMaster->setProperty("name", "master");
    ui->btnMaster->setText("MASTERNODES");
    ui->btnMaster->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    ui->btnSettings->setProperty("name", "settings");
    ui->btnSettings->setText("SETTINGS");
    ui->btnSettings->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    ui->btnReceive->setProperty("name", "receive");
    ui->btnReceive->setText("RECEIVE");
    ui->btnReceive->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    btns = {ui->btnDashboard, ui->btnSend, ui->btnReceive, ui->btnAddress, ui->btnMaster, ui->btnSettings, ui->btnHistory};
    onNavSelected(ui->btnDashboard, true);

    connectActions();
}

/**
 * Actions
 */
void NavMenuWidget::connectActions() {
    connect(ui->btnDashboard,SIGNAL(clicked()),this, SLOT(onDashboardClicked()));
    connect(ui->btnSend,SIGNAL(clicked()),this, SLOT(onSendClicked()));
    connect(ui->btnAddress,SIGNAL(clicked()),this, SLOT(onAddressClicked()));
    connect(ui->btnMaster,SIGNAL(clicked()),this, SLOT(onMasterNodesClicked()));
    connect(ui->btnSettings,SIGNAL(clicked()),this, SLOT(onSettingsClicked()));
    connect(ui->btnReceive,SIGNAL(clicked()),this, SLOT(onReceiveClicked()));
    connect(ui->btnHistory,SIGNAL(clicked()),this, SLOT(onHistoryClicked()));

    ui->btnDashboard->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_1));
    ui->btnSend->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_2));
    ui->btnReceive->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_3));
    ui->btnAddress->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_4));
    ui->btnMaster->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_6));
    ui->btnSettings->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_7));
    ui->btnHistory->setShortcut(QKeySequence(SHORT_KEY + Qt::Key_8));
}

void NavMenuWidget::onSendClicked(){
    window->goToSend();
    onNavSelected(ui->btnSend);
}

void NavMenuWidget::onDashboardClicked(){
    window->goToDashboard();
    onNavSelected(ui->btnDashboard);
}

void NavMenuWidget::onHistoryClicked(){
    window->goToHistory();
    onNavSelected(ui->btnHistory);
}

void NavMenuWidget::onAddressClicked(){
    window->goToAddresses();
    onNavSelected(ui->btnAddress);
}

void NavMenuWidget::onMasterNodesClicked(){
    window->goToMasterNodes();
    onNavSelected(ui->btnMaster);
}

void NavMenuWidget::onSettingsClicked(){
    window->goToSettings();
    onNavSelected(ui->btnSettings);
}

void NavMenuWidget::onReceiveClicked(){
    window->goToReceive();
    onNavSelected(ui->btnReceive);
}

void NavMenuWidget::onNavSelected(QWidget* active, bool startup) {
    QString start = "btn-nav-";
    foreach (QWidget* w, btns) {
        QString clazz = start + w->property("name").toString();
        if (w == active) {
            clazz += "-active";
        }
        setCssProperty(w, clazz);
    }
    if (!startup) updateButtonStyles();
}

void NavMenuWidget::selectSettings(){
    onSettingsClicked();
}

void NavMenuWidget::updateButtonStyles(){
    forceUpdateStyle({
         ui->btnDashboard,
         ui->btnSend,
         ui->btnAddress,
         ui->btnMaster,
         ui->btnSettings,
         ui->btnReceive,
         ui->btnHistory
    });
}

NavMenuWidget::~NavMenuWidget(){
    delete ui;
}
