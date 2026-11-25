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

const QString SELECT_BUDGETS = R"(
    SELECT b.id, b.nom, b.plafond, b.periode, b.date_debut, b.date_fin, b.seuil_alerte
    FROM budgets b
)";

// Les categories sont chargees a part, par chargerCategories()
Budget hydrateBudget(const QSqlQuery &query)
{
    return Budget(query.value(0).toInt(),
                  query.value(1).toString(),
                  query.value(2).toLongLong(),
                  query.value(3).toString(),
                  QDate::fromString(query.value(4).toString(), Qt::ISODate),
                  QDate::fromString(query.value(5).toString(), Qt::ISODate),
                  query.value(6).toInt());
}
} // namespace

bool BudgetRepository::ecrireCategories(int budgetId, const QList<int> &categoryIds, QString *errorMessage)
{
    QSqlQuery suppression(DatabaseManager::instance().database());
    suppression.prepare("DELETE FROM budget_categories WHERE budget_id = :budget_id;");
    suppression.bindValue(":budget_id", budgetId);
    if (!suppression.exec()) {
        setError(errorMessage, suppression.lastError().text());
        return false;
    }

    QSqlQuery insertion(DatabaseManager::instance().database());
    insertion.prepare("INSERT INTO budget_categories (budget_id, category_id) VALUES (:budget_id, :category_id);");
    for (int categoryId : categoryIds) {
        insertion.bindValue(":budget_id", budgetId);
        insertion.bindValue(":category_id", categoryId);
        if (!insertion.exec()) {
            setError(errorMessage, insertion.lastError().text());
            return false;
        }
    }
    return true;
}

bool BudgetRepository::chargerCategories(QList<Budget> &budgets, QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT category_id FROM budget_categories WHERE budget_id = :budget_id;");

    for (Budget &budget : budgets) {
        query.bindValue(":budget_id", budget.id());
        if (!query.exec()) {
            setError(errorMessage, query.lastError().text());
            return false;
        }
        QList<int> ids;
        while (query.next()) {
            ids.append(query.value(0).toInt());
        }
        budget.setCategoryIds(ids);
    }
    return true;
}

bool BudgetRepository::create(Budget &budget, QString *errorMessage)
{
    DatabaseManager &db = DatabaseManager::instance();
    if (!db.isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    // Le budget et ses categories sont enregistres ensemble, ou pas du tout
    if (!db.beginTransaction()) {
        setError(errorMessage, db.lastError());
        return false;
    }

    QSqlQuery query(db.database());
    query.prepare(R"(
        INSERT INTO budgets (nom, plafond, periode, date_debut, date_fin, seuil_alerte)
        VALUES (:nom, :plafond, :periode, :date_debut, :date_fin, :seuil_alerte);
    )");
    query.bindValue(":nom", budget.nom());
    query.bindValue(":plafond", budget.plafond());
    query.bindValue(":periode", budget.periode());
    query.bindValue(":date_debut", budget.dateDebut().toString(Qt::ISODate));
    query.bindValue(":date_fin", budget.dateFin().toString(Qt::ISODate));
    query.bindValue(":seuil_alerte", budget.seuilAlerte());

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        db.rollbackTransaction();
        return false;
    }

    const int id = query.lastInsertId().toInt();
    if (!ecrireCategories(id, budget.categoryIds(), errorMessage)) {
        db.rollbackTransaction();
        return false;
    }

    if (!db.commitTransaction()) {
        setError(errorMessage, db.lastError());
        db.rollbackTransaction();
        return false;
    }

    budget.setId(id);
    setError(errorMessage, QString());
    return true;
}

bool BudgetRepository::update(const Budget &budget, QString *errorMessage)
{
    DatabaseManager &db = DatabaseManager::instance();
    if (!db.isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    if (!db.beginTransaction()) {
        setError(errorMessage, db.lastError());
        return false;
    }

    QSqlQuery query(db.database());
    query.prepare(R"(
        UPDATE budgets
        SET nom = :nom,
            plafond = :plafond,
            periode = :periode,
            date_debut = :date_debut,
            date_fin = :date_fin,
            seuil_alerte = :seuil_alerte
        WHERE id = :id;
    )");
    query.bindValue(":id", budget.id());
    query.bindValue(":nom", budget.nom());
    query.bindValue(":plafond", budget.plafond());
    query.bindValue(":periode", budget.periode());
    query.bindValue(":date_debut", budget.dateDebut().toString(Qt::ISODate));
    query.bindValue(":date_fin", budget.dateFin().toString(Qt::ISODate));
    query.bindValue(":seuil_alerte", budget.seuilAlerte());

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        db.rollbackTransaction();
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucun budget trouve avec cet identifiant.");
        db.rollbackTransaction();
        return false;
    }

    if (!ecrireCategories(budget.id(), budget.categoryIds(), errorMessage)) {
        db.rollbackTransaction();
        return false;
    }

    if (!db.commitTransaction()) {
        setError(errorMessage, db.lastError());
        db.rollbackTransaction();
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

    // Les lignes de budget_categories partent avec (ON DELETE CASCADE)
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
    query.prepare(SELECT_BUDGETS + " WHERE b.id = :id;");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return Budget::null();
    }

    if (!query.next()) {
        setError(errorMessage, QString());
        return Budget::null();
    }

    QList<Budget> budgets = { hydrateBudget(query) };
    if (!chargerCategories(budgets, errorMessage)) {
        return Budget::null();
    }

    setError(errorMessage, QString());
    return budgets.first();
}

QList<Budget> BudgetRepository::findAll(QString *errorMessage) const
{
    QList<Budget> budgets;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return budgets;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(SELECT_BUDGETS + " ORDER BY b.date_debut DESC, b.id DESC;")) {
        setError(errorMessage, query.lastError().text());
        return budgets;
    }

    while (query.next()) {
        budgets.append(hydrateBudget(query));
    }

    if (!chargerCategories(budgets, errorMessage)) {
        return QList<Budget>();
    }

    setError(errorMessage, QString());
    return budgets;
}

QList<Budget> BudgetRepository::findByCategoryId(int categoryId, QString *errorMessage) const
{
    QList<Budget> budgets;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return budgets;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(SELECT_BUDGETS + R"(
        JOIN budget_categories bc ON bc.budget_id = b.id
        WHERE bc.category_id = :category_id
        ORDER BY b.date_debut DESC, b.id DESC;
    )");
    query.bindValue(":category_id", categoryId);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return budgets;
    }

    while (query.next()) {
        budgets.append(hydrateBudget(query));
    }

    if (!chargerCategories(budgets, errorMessage)) {
        return QList<Budget>();
    }

    setError(errorMessage, QString());
    return budgets;
}

qint64 BudgetRepository::montantDepense(int budgetId, QString *errorMessage) const
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return 0;
    }

    // Somme des depenses des categories du budget, entre sa date de debut et sa date de fin
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT COALESCE(SUM(t.montant), 0)
        FROM transactions t
        JOIN budget_categories bc ON bc.category_id = t.category_id
        JOIN budgets b ON b.id = bc.budget_id
        WHERE b.id = :id
          AND t.type = 'depense'
          AND t.date BETWEEN b.date_debut AND b.date_fin;
    )");
    query.bindValue(":id", budgetId);

    if (!query.exec() || !query.next()) {
        setError(errorMessage, query.lastError().text());
        return 0;
    }

    setError(errorMessage, QString());
    return query.value(0).toLongLong();
}
