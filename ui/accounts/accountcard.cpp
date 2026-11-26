#include "accountCard.h"
#include "ui_accountCard.h"
#include "money.h"

#include <QMouseEvent>
#include <QStyle>

AccountCard::AccountCard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountCard)
{
    ui->setupUi(this);
    setCursor(Qt::PointingHandCursor);

    if (auto *l = layout())
        l->setContentsMargins(0, 0, 0, 0);
    if (auto *l = ui->accCardFrame->layout()) {
        l->setContentsMargins(16, 12, 16, 12);
        l->setSpacing(12);
    }
    if (auto *l = ui->accNameBlock->layout()) {
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(2);
    }

    connect(ui->btnAccEdit, &QToolButton::clicked, this, [this] { emit editRequested(m_id); });
    connect(ui->btnAccDelete, &QToolButton::clicked, this, [this] { emit deleteRequested(m_id); });
}

AccountCard::~AccountCard()
{
    delete ui;
}

void AccountCard::setData(const AccountData &account)
{
    m_id = account.id;

    ui->lblAccName->setText(account.name);
    ui->lblAccBalance->setText(formatFcfa(account.balance));
    ui->lblAccMeta->setText(account.type + QStringLiteral(" · ")
                            + QString::number(account.monthTransactions)
                            + (account.monthTransactions > 1 ? QStringLiteral(" transactions ce mois")
                                                             : QStringLiteral(" transaction ce mois")));

    const QString color = account.color.isEmpty() ? QStringLiteral("#0F766E") : account.color;
    ui->lblAccIcon->setText(account.name.left(1).toUpper());
    ui->lblAccIcon->setStyleSheet(QStringLiteral("background: %1;").arg(color));
}

void AccountCard::setSelected(bool selected)
{
    ui->accCardFrame->setProperty("selected", selected);
    style()->unpolish(ui->accCardFrame);
    style()->polish(ui->accCardFrame);
}

void AccountCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked(m_id);
    QWidget::mousePressEvent(event);
}
