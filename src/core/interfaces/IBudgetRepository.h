#ifndef IBUDGETREPOSITORY_H
#define IBUDGETREPOSITORY_H

#include "../models/Budget.h"

#include <QList>
#include <QString>
#include <QtGlobal>

class IBudgetRepository
{
public:
    virtual ~IBudgetRepository() = default;

    // insere un nouveau budget avec ses categories et renseigne son id
    virtual bool create(Budget &budget, QString *errorMessage = nullptr) = 0;

    // met a jour un budget existant et la liste de ses categories
    virtual bool update(const Budget &budget, QString *errorMessage = nullptr) = 0;

    // supprime un budget par son identifiant
    virtual bool remove(int id, QString *errorMessage = nullptr) = 0;

    // recupere un budget par son identifiant, avec ses categories
    virtual Budget findById(int id, QString *errorMessage = nullptr) const = 0;

    // retourne tous les budgets, avec leurs categories
    virtual QList<Budget> findAll(QString *errorMessage = nullptr) const = 0;

    // retourne les budgets qui couvrent une categorie
    virtual QList<Budget> findByCategoryId(int categoryId, QString *errorMessage = nullptr) const = 0;

    // somme des depenses des categories du budget sur sa periode
    virtual qint64 montantDepense(int budgetId, QString *errorMessage = nullptr) const = 0;
};

#endif // IBUDGETREPOSITORY_H
