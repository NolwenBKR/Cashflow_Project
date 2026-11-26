#include "accountdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

AccountDialog::AccountDialog(QWidget *parent) :
    QDialog(parent)
{
    setObjectName("accountDialog");
    setWindowTitle(QStringLiteral("Nouveau compte"));
    setModal(true);
    setMinimumWidth(440);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    m_title = new QLabel(QStringLiteral("Nouveau compte"), this);
    m_title->setObjectName("lblDialogTitle");
    root->addWidget(m_title);

    auto *form = new QFormLayout;
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(14);

    // Nom
    m_name = new QLineEdit(this);
    m_name->setMaxLength(40);
    m_name->setPlaceholderText(QStringLiteral("Ex. : MTN MoMo"));
    form->addRow(QStringLiteral("Nom"), m_name);

    // Type
    m_type = new QComboBox(this);
    m_type->addItems({QStringLiteral("Mobile money"),
                      QStringLiteral("Épargne"),
                      QStringLiteral("Espèces")});
    form->addRow(QStringLiteral("Type"), m_type);

    // Couleur : une pastille par couleur
    auto *colorRow = new QHBoxLayout;
    colorRow->setSpacing(10);
    const QStringList colors = {
        QStringLiteral("#0F766E"), QStringLiteral("#F5B301"),
        QStringLiteral("#2563EB"), QStringLiteral("#475569"),
        QStringLiteral("#B42318"), QStringLiteral("#7C3AED")
    };
    for (const QString &c : colors) {
        auto *button = new QPushButton(this);
        button->setCheckable(true);
        button->setFixedSize(30, 30);
        button->setCursor(Qt::PointingHandCursor);
        button->setProperty("swatchColor", c);
        button->setStyleSheet(QStringLiteral(
            "QPushButton { background: %1; border: 2px solid transparent; border-radius: 15px; }"
            "QPushButton:checked { border: 3px solid #13201F; }").arg(c));
        connect(button, &QPushButton::clicked, this, [this, c] { selectColor(c); });
        m_swatches.append(button);
        colorRow->addWidget(button);
    }
    colorRow->addStretch();
    form->addRow(QStringLiteral("Couleur"), colorRow);

    // Solde
    m_balanceLabel = new QLabel(QStringLiteral("Solde initial"), this);
    m_balance = new QSpinBox(this);
    m_balance->setRange(0, 2000000000);
    m_balance->setSingleStep(1000);
    m_balance->setGroupSeparatorShown(true);
    m_balance->setSuffix(QStringLiteral(" FCFA"));
    m_balance->setButtonSymbols(QAbstractSpinBox::NoButtons);
    form->addRow(m_balanceLabel, m_balance);

    root->addLayout(form);

    // Message d'erreur
    m_error = new QLabel(this);
    m_error->setObjectName("lblDialogError");
    m_error->hide();
    root->addWidget(m_error);

    // Boutons
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *save = buttons->button(QDialogButtonBox::Ok);
    QPushButton *cancel = buttons->button(QDialogButtonBox::Cancel);
    save->setText(QStringLiteral("Enregistrer"));
    save->setObjectName("btnDialogSave");
    save->setDefault(true);
    cancel->setText(QStringLiteral("Annuler"));
    cancel->setObjectName("btnDialogCancel");
    connect(buttons, &QDialogButtonBox::accepted, this, &AccountDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);

    selectColor(colors.first());
    m_name->setFocus();
}

void AccountDialog::setAccount(const AccountData &account)
{
    m_id = account.id;
    setWindowTitle(QStringLiteral("Modifier le compte"));
    m_title->setText(QStringLiteral("Modifier le compte"));

    m_name->setText(account.name);

    const int typeIndex = m_type->findText(account.type);
    m_type->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);

    if (!account.color.isEmpty())
        selectColor(account.color);

    // Le solde d'un compte existant vient des transactions : on ne le modifie pas ici
    m_balanceLabel->setText(QStringLiteral("Solde actuel"));
    m_balance->setValue(int(qMin<qint64>(qMax<qint64>(account.balance, 0), 2000000000)));
    m_balance->setEnabled(false);
    m_balance->setToolTip(QStringLiteral("Le solde évolue avec les transactions."));
}

AccountData AccountDialog::account() const
{
    AccountData result;
    result.id = m_id;
    result.name = m_name->text().trimmed();
    result.type = m_type->currentText();
    result.color = m_color;
    result.balance = m_balance->value();
    return result;
}

void AccountDialog::accept()
{
    if (m_name->text().trimmed().isEmpty()) {
        m_error->setText(QStringLiteral("Le nom du compte est obligatoire."));
        m_error->show();
        m_name->setFocus();
        return;
    }
    QDialog::accept();
}

void AccountDialog::selectColor(const QString &color)
{
    m_color = color;
    for (QPushButton *button : m_swatches)
        button->setChecked(button->property("swatchColor").toString() == color);
}
