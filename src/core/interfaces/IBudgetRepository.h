#ifndef IBUDGETREPOSITORY_H
#define IBUDGETREPOSITORY_H

#include "../models/Budget.h"

#include <QList>
#include <QString>
#include <optional>

class IBudgetRepository
{
public:
    virtual ~IBudgetRepository() = default;

    // insere un nouveau budget et renseigne son id
    virtual bool create(Budget &budget, QString *errorMessage = nullptr) = 0;

    // met a jour un budget existant
    virtual bool update(const Budget &budget, QString *errorMessage = nullptr) = 0;

    // supprime un budget par son identifiant
    virtual bool remove(int id, QString *errorMessage = nullptr) = 0;

    // recupere un budget par son identifiant
    virtual Budget findById(int id, QString *errorMessage = nullptr) const = 0;

    // retourne tous les budgets
    virtual QList<Budget> findAll(QString *errorMessage = nullptr) const = 0;

    // filtre les budgets d'une categorie
    virtual QList<Budget> findByCategoryId(int categoryId, QString *errorMessage = nullptr) const = 0;

    // met a jour l'avancement d'un budget
    virtual bool updateProgress(int budgetId, int avancement, QString *errorMessage = nullptr) = 0;
};

#endif // IBUDGETREPOSITORY_H
