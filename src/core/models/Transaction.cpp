#include "Transaction.h"

Transaction::Transaction(int id,
                         const QDate &date,
                         const QTime &time,
                         const QString &nom,
                         const QString &description,
                         unsigned int montant,
                         int categoryId,
                         int accountId,
                         Type type)
    : m_date(date)
    , m_time(time)
    , m_nom(nom)
    , m_description(description)
    , m_montant(montant)
    , m_categoryId(categoryId)
    , m_accountId(accountId)
    , m_type(type)
{
    setId(id);
}

QDate Transaction::date() const
{
    return m_date;
}

void Transaction::setDate(const QDate &date)
{
    m_date = date;
}

QTime Transaction::time() const
{
    return m_time;
}

void Transaction::setTime(const QTime &time)
{
    m_time = time;
}

QString Transaction::nom() const
{
    return m_nom;
}

void Transaction::setNom(const QString &nom)
{
    m_nom = nom;
}

QString Transaction::description() const
{
    return m_description;
}

void Transaction::setDescription(const QString &description)
{
    m_description = description;
}

unsigned int Transaction::montant() const
{
    return m_montant;
}

void Transaction::setMontant(unsigned int montant)
{
    m_montant = montant;
}

qint64 Transaction::montantDelta() const
{
    const qint64 valeur = m_montant;
    return (m_type == Type::Approvisionnement) ? valeur : -valeur;
}

int Transaction::categoryId() const
{
    return m_categoryId;
}

void Transaction::setCategoryId(int categoryId)
{
    m_categoryId = categoryId;
}

int Transaction::accountId() const
{
    return m_accountId;
}

void Transaction::setAccountId(int accountId)
{
    m_accountId = accountId;
}

Transaction::Type Transaction::type() const
{
    return m_type;
}

void Transaction::setType(Type type)
{
    m_type = type;
}

QString Transaction::typeToString(Type type)
{
    switch (type) {
    case Type::Approvisionnement:
        return QStringLiteral("approvisionnement");
    case Type::Depense:
    default:
        return QStringLiteral("depense");
    }
}
int Transaction::recurrenceId() const
{
    return m_recurrenceId;
}

void Transaction::setRecurrenceId(int recurrenceId)
{
    m_recurrenceId = recurrenceId;
}

Transaction::Type Transaction::typeFromString(const QString &value, bool *ok)
{
    const QString normalized = value.trimmed().toLower();
    if (normalized == QStringLiteral("approvisionnement")) {
        if (ok) *ok = true;
        return Type::Approvisionnement;
    }
    if (normalized == QStringLiteral("depense") || normalized == QStringLiteral("dépense")) {
        if (ok) *ok = true;
        return Type::Depense;
    }

    if (ok) *ok = false;
    return Type::Depense;
}
