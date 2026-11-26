#ifndef ITRANSACTIONREPOSITORY_H
#define ITRANSACTIONREPOSITORY_H

#include "../models/Transaction.h"

#include <QDate>
#include <QList>
#include <QString>


class ITransactionRepository
{
public:
    virtual ~ITransactionRepository() = default;

    // insere une nouvelle transaction et renseigne son id
    virtual bool create(Transaction &transaction, QString *errorMessage = nullptr) = 0;

    // met a jour une transaction existante
    virtual bool update(const Transaction &transaction, QString *errorMessage = nullptr) = 0;

    // supprime une transaction par son identifiant
    virtual bool remove(int id, QString *errorMessage = nullptr) = 0;

    // recupere une transaction par son identifiant
    virtual Transaction findById(int id, QString *errorMessage = nullptr) const = 0;

    // retourne toutes les transactions
    virtual QList<Transaction> findAll(QString *errorMessage = nullptr) const = 0;

    // filtre les transactions par compte
    virtual QList<Transaction> findByAccountId(int accountId, QString *errorMessage = nullptr) const = 0;

    // filtre les transactions par categorie
    virtual QList<Transaction> findByCategoryId(int categoryId, QString *errorMessage = nullptr) const = 0;

    // filtre les transactions dans un intervalle de dates inclusif
    virtual QList<Transaction> findByDateRange(const QDate &startDate,
                                               const QDate &endDate,
                                               QString *errorMessage = nullptr) const = 0;
};

#endif // ITRANSACTIONREPOSITORY_H
