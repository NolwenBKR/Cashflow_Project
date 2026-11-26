#include "accountspage.h"
#include "ui_accountspage.h"
#include "accountcard.h"
#include "money.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

void clearLayout(QLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete item;
    }
}

QString colorOf(const AccountData &account)
{
    return account.color.isEmpty() ? QStringLiteral("#0F766E") : account.color;
}

} // namespace

AccountsPage::AccountsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountsPage)
{
    ui->setupUi(this);

    auto tidy = [](QWidget *w, int l, int t, int r, int b, int spacing) {
        if (auto *lay = w->layout()) {
            lay->setContentsMargins(l, t, r, b);
            lay->setSpacing(spacing);
        }
    };
    tidy(this,           32, 32, 32, 32, 20);
    tidy(ui->headerRow,   0,  0,  0,  0, 12);
    tidy(ui->titleBlock,  0,  0,  0,  0,  4);
    tidy(ui->totalCard,  20, 16, 20, 16,  8);
    tidy(ui->bodyRow,     0,  0,  0,  0, 20);
    tidy(ui->detailCard, 20, 16, 20, 16,  8);
    tidy(ui->inOutRow,    0,  0,  0,  0, 12);
    tidy(ui->inBox,      12,  8, 12,  8,  2);
    tidy(ui->outBox,     12,  8, 12,  8,  2);

    m_listLayout = new QVBoxLayout(ui->accountsPanel);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);

    m_barLayout = new QHBoxLayout(ui->balanceBar);
    m_barLayout->setContentsMargins(0, 0, 0, 0);
    m_barLayout->setSpacing(0);

    m_opsLayout = new QVBoxLayout(ui->lastOpsBox);
    m_opsLayout->setContentsMargins(0, 0, 0, 0);
    m_opsLayout->setSpacing(0);

    ui->lblIn->setProperty("tone", "income");
    ui->lblOut->setProperty("tone", "expense");

    connect(ui->btnNewAccount, &QPushButton::clicked,
            this, &AccountsPage::newAccountRequested);
    connect(ui->btnSeeTransactions, &QPushButton::clicked, this, [this] {
        if (m_selectedId >= 0)
            emit showTransactionsRequested(m_selectedId);
    });

    setAccounts(QVector<AccountData>());
}

AccountsPage::~AccountsPage()
{
    delete ui;
}

void AccountsPage::setAccounts(const QVector<AccountData> &accounts)
{
    m_accounts = accounts;

    // Solde total
    qint64 total = 0;
    for (const AccountData &account : m_accounts)
        total += account.balance;
    ui->lblTotalValue->setText(formatFcfa(total));

    // Barre de répartition : un segment par compte au solde positif
    clearLayout(m_barLayout);
    QVector<const AccountData *> positive;
    for (const AccountData &account : m_accounts) {
        if (account.balance > 0)
            positive.append(&account);
    }
    for (int i = 0; i < positive.size(); ++i) {
        auto *segment = new QFrame(ui->balanceBar);
        QString style = QStringLiteral("background: %1;").arg(colorOf(*positive[i]));
        if (i == 0)
            style += QStringLiteral("border-top-left-radius: 6px; border-bottom-left-radius: 6px;");
        if (i == positive.size() - 1)
            style += QStringLiteral("border-top-right-radius: 6px; border-bottom-right-radius: 6px;");
        segment->setStyleSheet(style);
        m_barLayout->addWidget(segment, int(qMin<qint64>(positive[i]->balance, 1000000000)));
    }

    // Cartes
    clearLayout(m_listLayout);
    m_cards.clear();

    if (m_accounts.isEmpty()) {
        auto *empty = new QLabel(QStringLiteral(
            "Aucun compte pour le moment.\nCliquez sur « + Nouveau compte » pour commencer."),
            ui->accountsPanel);
        empty->setObjectName("lblAccountsEmpty");
        m_listLayout->addWidget(empty);
    }

    for (const AccountData &account : m_accounts) {
        auto *card = new AccountCard(ui->accountsPanel);
        card->setData(account);
        connect(card, &AccountCard::clicked, this, [this](int id) {
            m_selectedId = id;
            applySelection();
            showDetail(id);
        });
        connect(card, &AccountCard::editRequested, this, &AccountsPage::editAccountRequested);
        connect(card, &AccountCard::deleteRequested, this, [this](int id) {
            QString name;
            for (const AccountData &acc : m_accounts) {
                if (acc.id == id)
                    name = acc.name;
            }
            const auto answer = QMessageBox::question(
                this, QStringLiteral("Supprimer le compte"),
                QStringLiteral("Supprimer le compte « %1 » ?\nCette action est irréversible.").arg(name),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (answer == QMessageBox::Yes)
                emit deleteAccountRequested(id);
        });
        m_listLayout->addWidget(card);
        m_cards.append(card);
    }
    m_listLayout->addStretch();

    // Sélection : on garde le compte choisi s'il existe encore, sinon le premier
    bool found = false;
    for (const AccountData &account : m_accounts) {
        if (account.id == m_selectedId)
            found = true;
    }
    if (!found)
        m_selectedId = m_accounts.isEmpty() ? -1 : m_accounts.first().id;

    applySelection();
    if (m_selectedId >= 0)
        showDetail(m_selectedId);
    else
        clearDetail();
}

void AccountsPage::showDetail(int id)
{
    const AccountData *account = nullptr;
    for (const AccountData &a : m_accounts) {
        if (a.id == id)
            account = &a;
    }
    if (!account) {
        clearDetail();
        return;
    }

    ui->lblDetailTitle->setText(QStringLiteral("Détail : ") + account->name);
    ui->lblDetailType->setText(account->type);
    ui->lblDetailBalance->setText(formatFcfa(account->balance));
    ui->lblIn->setText(formatFcfa(account->monthIn, true));
    ui->lblOut->setText(formatFcfa(account->monthOut));
    ui->btnSeeTransactions->setEnabled(true);

    clearLayout(m_opsLayout);
    int shown = 0;
    for (const AccountOperation &op : account->recent) {
        if (shown >= 5)
            break;
        auto *row = new QWidget(ui->lastOpsBox);
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 6, 0, 6);
        auto *name = new QLabel(op.label, row);
        auto *amount = new QLabel(formatFcfa(op.amount, true), row);
        amount->setProperty("tone", op.amount >= 0 ? "income" : "expense");
        h->addWidget(name, 1);
        h->addWidget(amount);
        m_opsLayout->addWidget(row);
        ++shown;
    }
    if (shown == 0) {
        auto *none = new QLabel(QStringLiteral("Aucune opération pour ce compte."), ui->lastOpsBox);
        none->setObjectName("lblAccountsNoOps");
        m_opsLayout->addWidget(none);
    }
}

void AccountsPage::applySelection()
{
    for (AccountCard *card : m_cards)
        card->setSelected(card->accountId() == m_selectedId);
}

void AccountsPage::clearDetail()
{
    ui->lblDetailTitle->setText(QStringLiteral("Aucun compte sélectionné"));
    ui->lblDetailType->clear();
    ui->lblDetailBalance->setText(formatFcfa(0));
    ui->lblIn->setText(formatFcfa(0));
    ui->lblOut->setText(formatFcfa(0));
    ui->btnSeeTransactions->setEnabled(false);
    clearLayout(m_opsLayout);
}
