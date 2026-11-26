#ifndef BUDGETREPOSITORY_H
#define BUDGETREPOSITORY_H

#include "../../core/interfaces/IBudgetRepository.h"

class BudgetRepository : public IBudgetRepository
{
public:
    BudgetRepository() = default;

    bool create(Budget &budget, QString *errorMessage = nullptr) override;
    bool update(const Budget &budget, QString *errorMessage = nullptr) override;
    bool remove(int id, QString *errorMessage = nullptr) override;
    Budget findById(int id, QString *errorMessage = nullptr) const override;
    QList<Budget> findAll(QString *errorMessage = nullptr) const override;
    QList<Budget> findByCategoryId(int categoryId, QString *errorMessage = nullptr) const override;
    qint64 montantDepense(int budgetId, QString *errorMessage = nullptr) const override;

private:
    // remplace les categories d'un budget (a appeler dans une transaction SQL)
    bool ecrireCategories(int budgetId, const QList<int> &categoryIds, QString *errorMessage);
    // complete chaque budget avec la liste de ses categories
    bool chargerCategories(QList<Budget> &budgets, QString *errorMessage) const;
};

#endif // BUDGETREPOSITORY_H
