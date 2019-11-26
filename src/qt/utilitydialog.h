// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2017-2018 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ALQO_QT_UTILITYDIALOG_H
#define ALQO_QT_UTILITYDIALOG_H

#include <QDialog>
#include <QObject>
#include <QMainWindow>

class ClientModel;

namespace Ui
{
class HelpMessageDialog;
}

/** "Help message" dialog box */
class HelpMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpMessageDialog(QWidget* parent, bool about);
    ~HelpMessageDialog();

    void printToConsole();
    void showOrPrint();

private:
    Ui::HelpMessageDialog* ui;
    QString text;
};


/** "Shutdown" window */
class ShutdownWindow : public QWidget
{
    Q_OBJECT

public:
    ShutdownWindow(QWidget* parent = 0, Qt::WindowFlags f = 0);
    static void showShutdownWindow(QMainWindow* window);

protected:
    void closeEvent(QCloseEvent* event);
};

#endif // ALQO_QT_UTILITYDIALOG_H
