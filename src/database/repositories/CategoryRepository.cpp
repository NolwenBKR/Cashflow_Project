#include "CategoryRepository.h"
#include "../DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {
void setError(QString *errorMessage, const QString &message)
{
    if (errorMessage) {
        *errorMessage = message;
    }
}
} // namespace

bool CategoryRepository::create(Category &category, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO categories (nom, couleur, type) VALUES (:nom, :couleur, :type);");
    query.bindValue(":nom", category.nom());
    query.bindValue(":couleur", category.couleur().isEmpty() ? QVariant() : QVariant(category.couleur()));
    query.bindValue(":type", category.type());

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    category.setId(query.lastInsertId().toInt());
    setError(errorMessage, QString());
    return true;
}

bool CategoryRepository::update(const Category &category, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE categories SET nom = :nom, couleur = :couleur, type = :type WHERE id = :id;");
    query.bindValue(":id", category.id());
    query.bindValue(":nom", category.nom());
    query.bindValue(":couleur", category.couleur().isEmpty() ? QVariant() : QVariant(category.couleur()));
    query.bindValue(":type", category.type());

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucune categorie trouvee avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

bool CategoryRepository::remove(int id, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM categories WHERE id = :id;");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucune categorie trouvee avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

Category CategoryRepository::findById(int id, QString *errorMessage) const
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return Category::null();
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT id, nom, couleur, type FROM categories.");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return Category::null();
    }

    setError(errorMessage, QString());
    if (!query.next()) {
        return Category::null();
    }

    return Category(query.value(0).toInt(),
                    query.value(1).toString(),
                    query.value(2).toString(),
                    query.value(3).toString());
}
QList<Category> CategoryRepository::findAll(QString *errorMessage) const
{
    QList<Category> categories;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return categories;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec("SELECT id, nom, couleur, type FROM categories ORDER BY nom ASC;")) {
        setError(errorMessage, query.lastError().text());
        return categories;
    }

    while (query.next()) {
        categories.append(Category(query.value(0).toInt(),
                                   query.value(1).toString(),
                                   query.value(2).toString(),
                                   query.value(3).toString()));
    }

    setError(errorMessage, QString());
    return categories;
}

QList<Category> CategoryRepository::findByType(const QString &type, QString *errorMessage) const
{
    QList<Category> categories;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return categories;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT id, nom, couleur, type FROM categories WHERE type = :type ORDER BY nom ASC;");
    query.bindValue(":type", type);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return categories;
    }

    while (query.next()) {
        categories.append(Category(query.value(0).toInt(),
                                   query.value(1).toString(),
                                   query.value(2).toString(),
                                   query.value(3).toString()));
    }

    setError(errorMessage, QString());
    return categories;
}
