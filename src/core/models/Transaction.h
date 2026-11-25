#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "BaseModel.h"

#include <QDate>
#include <QString>
#include <QTime>
#include <QtGlobal>

class Transaction : public BaseModel<Transaction>
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

    QDate date() const;
    void setDate(const QDate &date);

    QTime time() const;
    void setTime(const QTime &time);

    QString nom() const;
    void setNom(const QString &nom);

    QString description() const;
    void setDescription(const QString &description);

    qint64 montant() const;
    void setMontant(qint64 montant);
    // retourne +montant pour un approvisionnement et -montant pour une depense
    qint64 montantDelta() const;

    int categoryId() const;
    void setCategoryId(int categoryId);

    int accountId() const;
    void setAccountId(int accountId);

    Type type()const;
    void setType(Type type);


    int recurrenceId() const;
    void setRecurrenceId(int recurrenceId);

    // helpers pratiques pour la conversion avec les valeurs SQL
    static QString typeToString(Type type);
    static Type typeFromString(const QString &value, bool *ok = nullptr);

private:
    QDate m_date;
    QTime m_time;
    QString m_nom;
    QString m_description;
    qint64 m_montant = 0;
    int m_categoryId = 0;
    int m_accountId = 0;
    Type m_type = Type::Depense;
    int m_recurrenceId = 0;
};

#endif // TRANSACTION_H
