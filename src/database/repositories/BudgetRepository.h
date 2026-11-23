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
    bool updateProgress(int budgetId, int avancement, QString *errorMessage = nullptr) override;
};

#endif // BUDGETREPOSITORY_H
