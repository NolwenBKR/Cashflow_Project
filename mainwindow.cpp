
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "accountspage.h"
#include "accountdialog.h"
#include "AccountData.h"

#include <QDate>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->navList->clear();
    ui->navList->addItems({
        "Tableau de bord",
        "Transactions",
        "Comptes",
        "Catégories & étiquettes",
        "Budgets",
        "Récurrences",
        "Rapports",
        "Bourse",
        "Import / Export"
    });

    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->centralwidget->layout()->setSpacing(0);
    auto *sideLayout = qobject_cast<QVBoxLayout *>(ui->sidebar->layout());
    if (sideLayout) {
        sideLayout->removeWidget(ui->sidebarSeparator);
        sideLayout->insertWidget(sideLayout->indexOf(ui->btnSettings), ui->sidebarSeparator);
    }

    // ---- Page Comptes (index 2) : on remplace la page vide de Designer ----
    auto *accountsPage = new AccountsPage(this);
    QWidget *oldPage = ui->stackedPages->widget(2);
    ui->stackedPages->removeWidget(oldPage);
    oldPage->deleteLater();
    ui->stackedPages->insertWidget(2, accountsPage);

    // Données de test, à remplacer par la base plus tard
    const QDate today = QDate::currentDate();
    QVector<AccountData> demo;

    auto addOp = [](AccountData &acc, const QString &label, qint64 amount, const QDate &date) {
        AccountOperation op;
        op.label = label;
        op.amount = amount;
        op.date = date;
        acc.recent.append(op);
    };

    AccountData momo;
    momo.id = 1;
    momo.name = QStringLiteral("MTN MoMo");
    momo.type = QStringLiteral("Mobile money");
    momo.color = QStringLiteral("#F5B301");
    momo.balance = 185000;
    momo.monthTransactions = 18;
    momo.monthIn = 120000;
    momo.monthOut = -64500;
    addOp(momo, QStringLiteral("Courses marché"), -12500, today.addDays(-1));
    addOp(momo, QStringLiteral("Salaire"), 120000, today.addDays(-3));
    addOp(momo, QStringLiteral("Transport"), -2500, today.addDays(-4));
    demo.append(momo);

    AccountData epargne;
    epargne.id = 2;
    epargne.name = QStringLiteral("Épargne");
    epargne.type = QStringLiteral("Épargne");
    epargne.color = QStringLiteral("#0F766E");
    epargne.balance = 450000;
    epargne.monthTransactions = 2;
    epargne.monthIn = 50000;
    epargne.monthOut = 0;
    addOp(epargne, QStringLiteral("Virement mensuel"), 50000, today.addDays(-5));
    demo.append(epargne);

    AccountData cash;
    cash.id = 3;
    cash.name = QStringLiteral("Espèces");
    cash.type = QStringLiteral("Espèces");
    cash.color = QStringLiteral("#475569");
    cash.balance = 32000;
    cash.monthTransactions = 7;
    cash.monthIn = 10000;
    cash.monthOut = -28000;
    addOp(cash, QStringLiteral("Repas"), -3500, today.addDays(-2));
    demo.append(cash);

    AccountData moov;
    moov.id = 4;
    moov.name = QStringLiteral("Moov Money");
    moov.type = QStringLiteral("Mobile money");
    moov.color = QStringLiteral("#2563EB");
    moov.balance = 54000;
    moov.monthTransactions = 5;
    moov.monthIn = 20000;
    moov.monthOut = -15000;
    demo.append(moov);

    m_accountsPage = accountsPage;
    m_accounts = demo;
    m_accountsPage->setAccounts(m_accounts);

    connect(accountsPage, &AccountsPage::newAccountRequested, this, [this] {
        openAccountDialog(-1);
    });
    connect(accountsPage, &AccountsPage::editAccountRequested, this, [this](int id) {
        openAccountDialog(id);
    });
    connect(accountsPage, &AccountsPage::deleteAccountRequested, this, [this](int id) {
        for (int i = 0; i < m_accounts.size(); ++i) {
            if (m_accounts[i].id == id) {
                m_accounts.removeAt(i);
                break;
            }
        }
        m_accountsPage->setAccounts(m_accounts);
    });
    connect(accountsPage, &AccountsPage::showTransactionsRequested, this, [this](int) {
        ui->navList->setCurrentRow(1);
    });

    // ---- Navigation ----
    connect(ui->navList, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row < 0) return;
        ui->btnSettings->setChecked(false);
        ui->stackedPages->setCurrentIndex(row);
    });
    connect(ui->btnSettings, &QPushButton::clicked, this, [this] {
        ui->btnSettings->setChecked(true);
        ui->navList->clearSelection();
        ui->navList->setCurrentRow(-1);
        ui->stackedPages->setCurrentIndex(9);
    });
    ui->navList->setCurrentRow(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openAccountDialog(int id)
{
    AccountDialog dialog(this);

    int index = -1;
    if (id >= 0) {
        for (int i = 0; i < m_accounts.size(); ++i) {
            if (m_accounts[i].id == id)
                index = i;
        }
        if (index < 0)
            return;
        dialog.setAccount(m_accounts[index]);
    }

    if (dialog.exec() != QDialog::Accepted)
        return;

    const AccountData result = dialog.account();
    if (index >= 0) {
        AccountData &existing = m_accounts[index];
        existing.name = result.name;
        existing.type = result.type;
        existing.color = result.color;
    } else {
        AccountData created = result;
        int maxId = 0;
        for (const AccountData &a : m_accounts)
            maxId = qMax(maxId, a.id);
        created.id = maxId + 1;
        m_accounts.append(created);
    }

    m_accountsPage->setAccounts(m_accounts);
}
