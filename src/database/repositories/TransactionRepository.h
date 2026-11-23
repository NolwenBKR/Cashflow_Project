#ifndef TRANSACTIONREPOSITORY_H
#define TRANSACTIONREPOSITORY_H

#include "../../core/interfaces/ITransactionRepository.h"

class TransactionRepository : public ITransactionRepository
{
public:
    TransactionRepository() = default;

    bool create(Transaction &transaction, QString *errorMessage = nullptr) override;
    bool update(const Transaction &transaction, QString *errorMessage = nullptr) override;
    bool remove(int id, QString *errorMessage = nullptr) override;
    Transaction findById(int id, QString *errorMessage = nullptr) const override;
    QList<Transaction> findAll(QString *errorMessage = nullptr) const override;
    QList<Transaction> findByAccountId(int accountId, QString *errorMessage = nullptr) const override;
    QList<Transaction> findByCategoryId(int categoryId, QString *errorMessage = nullptr) const override;
    QList<Transaction> findByDateRange(const QDate &startDate,
                                       const QDate &endDate,
                                       QString *errorMessage = nullptr) const override;
};

#endif // TRANSACTIONREPOSITORY_H
