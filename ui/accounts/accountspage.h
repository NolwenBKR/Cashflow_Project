#ifndef ACCOUNTSPAGE_H
#define ACCOUNTSPAGE_H

#include <QVector>
#include <QWidget>
#include "AccountData.h"

namespace Ui { class AccountsPage; }
class AccountCard;
class QHBoxLayout;
class QVBoxLayout;

class AccountsPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsPage(QWidget *parent = nullptr);
    ~AccountsPage();

public slots:
    void setAccounts(const QVector<AccountData> &accounts);

signals:
    void newAccountRequested();
    void editAccountRequested(int id);
    void deleteAccountRequested(int id);     // émis après confirmation
    void showTransactionsRequested(int id);

private:
    void showDetail(int id);
    void clearDetail();
    void applySelection();

    Ui::AccountsPage *ui;
    QVector<AccountData> m_accounts;
    QVector<AccountCard *> m_cards;
    QVBoxLayout *m_listLayout = nullptr;
    QHBoxLayout *m_barLayout = nullptr;
    QVBoxLayout *m_opsLayout = nullptr;
    int m_selectedId = -1;
};

#endif // ACCOUNTSPAGE_H
