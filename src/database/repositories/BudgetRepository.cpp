#include "BudgetRepository.h"
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

Budget hydrateBudget(const QSqlQuery &query)
{
    return Budget(query.value(0).toInt(),
                  query.value(1).toInt(),
                  query.value(2).toInt(),
                  query.value(3).toInt(),
                  QDate::fromString(query.value(4).toString(), Qt::ISODate),
                  QDate::fromString(query.value(5).toString(), Qt::ISODate));
}
} // namespace

bool BudgetRepository::create(Budget &budget, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO budgets (plafond, avancement, category_id, date_debut, date_fin)
        VALUES (:plafond, :avancement, :category_id, :date_debut, :date_fin);
    )");
    query.bindValue(":plafond", budget.plafond());
    query.bindValue(":avancement", budget.avancement());
    query.bindValue(":category_id", budget.categoryId());
    query.bindValue(":date_debut", budget.dateDebut().toString(Qt::ISODate));
    query.bindValue(":date_fin", budget.dateFin().toString(Qt::ISODate));

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    budget.setId(query.lastInsertId().toInt());
    setError(errorMessage, QString());
    return true;
}

bool BudgetRepository::update(const Budget &budget, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        UPDATE budgets
        SET plafond = :plafond,
            avancement = :avancement,
            category_id = :category_id,
            date_debut = :date_debut,
            date_fin = :date_fin
        WHERE id = :id;
    )");
    query.bindValue(":id", budget.id());
    query.bindValue(":plafond", budget.plafond());
    query.bindValue(":avancement", budget.avancement());
    query.bindValue(":category_id", budget.categoryId());
    query.bindValue(":date_debut", budget.dateDebut().toString(Qt::ISODate));
    query.bindValue(":date_fin", budget.dateFin().toString(Qt::ISODate));

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucun budget trouve avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

bool BudgetRepository::remove(int id, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM budgets WHERE id = :id;");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucun budget trouve avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

Budget BudgetRepository::findById(int id, QString *errorMessage) const
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return Budget::null();
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, plafond, avancement, category_id, date_debut, date_fin
        FROM budgets
        WHERE id = :id;
    )");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return Budget::null();
    }

    setError(errorMessage, QString());
    if (!query.next()) {
        return Budget::null();
    }

    return hydrateBudget(query);
}

QList<Budget> BudgetRepository::findAll(QString *errorMessage) const
{
    QList<Budget> items;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return items;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(R"(
        SELECT id, plafond, avancement, category_id, date_debut, date_fin
        FROM budgets
        ORDER BY date_debut DESC, id DESC;
    )")) {
        setError(errorMessage, query.lastError().text());
        return items;
    }

    while (query.next()) {
        items.append(hydrateBudget(query));
    }

    setError(errorMessage, QString());
    return items;
}

QList<Budget> BudgetRepository::findByCategoryId(int categoryId, QString *errorMessage) const
{
    QList<Budget> items;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return items;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, plafond, avancement, category_id, date_debut, date_fin
        FROM budgets
        WHERE category_id = :category_id
        ORDER BY date_debut DESC, id DESC;
    )");
    query.bindValue(":category_id", categoryId);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return items;
    }

    while (query.next()) {
        items.append(hydrateBudget(query));
    }

    setError(errorMessage, QString());
    return items;
}

bool BudgetRepository::updateProgress(int budgetId, int avancement, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE budgets SET avancement = :avancement WHERE id = :id;");
    query.bindValue(":id", budgetId);
    query.bindValue(":avancement", avancement);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucun budget trouve avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}
