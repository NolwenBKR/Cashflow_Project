#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDate>
#include <QString>
#include <QTime>

class Transaction
{
public:
    enum class Type {
        Depense,
        Approvisionnement
    };

    Transaction() = default;
    Transaction(int id,
                const QDate &date,
                const QTime &time,
                const QString &nom,
                const QString &description,
                unsigned int montant,
                int categoryId,
                int accountId,
                Type type);

    int id() const;
    void setId(int id);

    QDate date() const;
    void setDate(const QDate &date);

    QTime time() const;
    void setTime(const QTime &time);

    QString nom() const;
    void setNom(const QString &nom);

    QString description() const;
    void setDescription(const QString &description);

    unsigned int montant() const;
    void setMontant(unsigned int montant);
    // retourne +montant pour un approvisionnement et -montant pour une depense
    long long montantDelta() const;

    int categoryId() const;
    void setCategoryId(int categoryId);

    int accountId() const;
    void setAccountId(int accountId);

    Type type() const;
    void setType(Type type);

    // helpers pratiques pour la conversion avec les valeurs SQL
    static QString typeToString(Type type);
    static Type typeFromString(const QString &value, bool *ok = nullptr);

private:
    int m_id = 0;
    QDate m_date;
    QTime m_time;
    QString m_nom;
    QString m_description;
    unsigned int m_montant = 0;
    int m_categoryId = 0;
    int m_accountId = 0;
    Type m_type = Type::Depense;
};

#endif // TRANSACTION_H
