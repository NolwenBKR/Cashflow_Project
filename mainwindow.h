#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "AccountData.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class AccountsPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void openAccountDialog(int id);

    Ui::MainWindow *ui;
    QVector<AccountData> m_accounts;
    AccountsPage *m_accountsPage = nullptr;
};

#endif // MAINWINDOW_H
