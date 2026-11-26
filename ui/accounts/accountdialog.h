#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>
#include <QVector>
#include "AccountData.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class AccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountDialog(QWidget *parent = nullptr);

    void setAccount(const AccountData &account);   // mode modification
    AccountData account() const;                   // valeurs saisies

    void accept() override;

private:
    void selectColor(const QString &color);

    int m_id = -1;
    QString m_color;
    QLabel *m_title = nullptr;
    QLabel *m_error = nullptr;
    QLabel *m_balanceLabel = nullptr;
    QLineEdit *m_name = nullptr;
    QComboBox *m_type = nullptr;
    QSpinBox *m_balance = nullptr;
    QVector<QPushButton *> m_swatches;
};

#endif // ACCOUNTDIALOG_H
