#ifndef CATEGORYREPOSITORY_H
#define CATEGORYREPOSITORY_H

#include "../../core/interfaces/ICategoryRepository.h"

class CategoryRepository : public ICategoryRepository
{
public:
    CategoryRepository() = default;

    bool create(Category &category, QString *errorMessage = nullptr) override;
    bool update(const Category &category, QString *errorMessage = nullptr) override;
    bool remove(int id, QString *errorMessage = nullptr) override;
    Category findById(int id, QString *errorMessage = nullptr) const override;
    QList<Category> findAll(QString *errorMessage = nullptr) const override;
    QList<Category> findByType(const QString &type, QString *errorMessage = nullptr) const override;
};

#endif // CATEGORYREPOSITORY_H
