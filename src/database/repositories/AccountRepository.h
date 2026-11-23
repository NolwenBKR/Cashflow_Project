#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

#include "../../core/interfaces/IAccountRepository.h"

class AccountRepository : public IAccountRepository
{
public:
    AccountRepository() = default;

    bool create(Compte &compte, QString *errorMessage = nullptr) override;
    bool update(const Compte &compte, QString *errorMessage = nullptr) override;
    bool remove(int id, QString *errorMessage = nullptr) override;
    Compte findById(int id, QString *errorMessage = nullptr) const override;
    QList<Compte> findAll(QString *errorMessage = nullptr) const override;
    bool updateBalance(int accountId, int newBalance, QString *errorMessage = nullptr) override;
};

#endif // ACCOUNTREPOSITORY_H
