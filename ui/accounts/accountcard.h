#ifndef ACCOUNTCARD_H
#define ACCOUNTCARD_H

#include <QWidget>
#include "accountData.h"

namespace Ui { class AccountCard; }

class AccountCard : public QWidget
{
    Q_OBJECT

public:
    explicit AccountCard(QWidget *parent = nullptr);
    ~AccountCard();

    void setData(const AccountData &account);
    void setSelected(bool selected);
    int accountId() const { return m_id; }

signals:
    void clicked(int id);
    void editRequested(int id);
    void deleteRequested(int id);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::AccountCard *ui;
    int m_id = -1;
};

#endif // ACCOUNTCARD_H
