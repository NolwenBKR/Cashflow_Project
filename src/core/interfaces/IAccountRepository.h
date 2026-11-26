#ifndef IACCOUNTREPOSITORY_H
#define IACCOUNTREPOSITORY_H

#include "../models/Compte.h"

#include <QList>
#include <QString>

class IAccountRepository
{
public:
    virtual ~IAccountRepository() = default;

    // insere un nouveau compte (place en dernier) et met a jour son id et son ordre
    virtual bool create(Compte &compte, QString *errorMessage = nullptr) = 0;

    // met a jour nom, type, solde initial et couleur d'un compte existant
    virtual bool update(const Compte &compte, QString *errorMessage = nullptr) = 0;

    // supprime un compte par son identifiant (ses transactions sont supprimees aussi)
    virtual bool remove(int id, QString *errorMessage = nullptr) = 0;

    // recupere un compte par son identifiant, avec son solde calcule
    virtual Compte findById(int id, QString *errorMessage = nullptr) const = 0;

    // retourne tous les comptes, tries selon l'ordre choisi par l'utilisateur
    virtual QList<Compte> findAll(QString *errorMessage = nullptr) const = 0;

    // enregistre un nouvel ordre d'affichage : la position dans la liste devient l'ordre
    virtual bool updateOrder(const QList<int> &idsDansLOrdre, QString *errorMessage = nullptr) = 0;
};

#endif // IACCOUNTREPOSITORY_H
