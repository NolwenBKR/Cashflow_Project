#ifndef IACCOUNTREPOSITORY_H
#define IACCOUNTREPOSITORY_H

#include "../models/Compte.h"

#include <QList>
#include <QString>
#include <optional>

class IAccountRepository
{
public:
    virtual ~IAccountRepository() = default;

    // insere un nouveau compte et met a jour son id en cas de succes
    virtual bool create(Compte &compte, QString *errorMessage = nullptr) = 0;

    // met a jour un compte existant
    virtual bool update(const Compte &compte, QString *errorMessage = nullptr) = 0;

    // supprime un compte par son identifiant
    virtual bool remove(int id, QString *errorMessage = nullptr) = 0;

    // recupere un compte par son identifiant
    virtual Compte findById(int id, QString *errorMessage = nullptr) const = 0;

    // retourne tous les comptes
    virtual QList<Compte> findAll(QString *errorMessage = nullptr) const = 0;

    // met a jour uniquement le solde d'un compte
    virtual bool updateBalance(int accountId, int newBalance, QString *errorMessage = nullptr) = 0;
};

#endif // IACCOUNTREPOSITORY_H
