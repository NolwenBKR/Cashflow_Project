#ifndef ICATEGORYREPOSITORY_H
#define ICATEGORYREPOSITORY_H

#include "../models/Category.h"

#include <QList>
#include <QString>
#include <optional>

class ICategoryRepository
{
public:
    virtual ~ICategoryRepository() = default;

    // insere une nouvelle categorie et renseigne son id
    virtual bool create(Category &category, QString *errorMessage = nullptr) = 0;

    // met a jour une categorie existante
    virtual bool update(const Category &category, QString *errorMessage = nullptr) = 0;

    // supprime une categorie par son identifiant
    virtual bool remove(int id, QString *errorMessage = nullptr) = 0;

    // recupere une categorie par son identifiant
    virtual Category findById(int id, QString *errorMessage = nullptr) const = 0;

    // retourne toutes les categories
    virtual QList<Category> findAll(QString *errorMessage = nullptr) const = 0;
};

#endif // ICATEGORYREPOSITORY_H
