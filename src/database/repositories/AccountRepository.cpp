#include "AccountRepository.h"
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

// Lit les comptes avec leur solde calcule : solde initial + revenus - depenses.
// LEFT JOIN garde les comptes sans transaction, COALESCE remplace une somme vide par 0.
const QString SELECT_COMPTES = R"(
    SELECT a.id, a.nom, a.type_de_compte, a.solde_initial, a.couleur, a.ordre,
           a.solde_initial + COALESCE(SUM(CASE WHEN t.type = 'approvisionnement'
                                               THEN t.montant ELSE -t.montant END), 0) AS solde
    FROM accounts a
    LEFT JOIN transactions t ON t.account_id = a.id
)";

Compte hydrateCompte(const QSqlQuery &query)
{
    return Compte(query.value(0).toInt(),
                  query.value(1).toString(),
                  query.value(2).toString(),
                  query.value(3).toLongLong(),
                  query.value(4).toString(),
                  query.value(5).toInt(),
                  query.value(6).toLongLong());
}

QVariant couleurOuNull(const QString &couleur)
{
    return couleur.isEmpty() ? QVariant() : QVariant(couleur);
}
} // namespace

bool AccountRepository::create(Compte &compte, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    // Le nouveau compte se place apres tous les autres
    QSqlQuery ordreQuery(DatabaseManager::instance().database());
    if (!ordreQuery.exec("SELECT COALESCE(MAX(ordre), 0) + 1 FROM accounts;") || !ordreQuery.next()) {
        setError(errorMessage, ordreQuery.lastError().text());
        return false;
    }
    const int ordre = ordreQuery.value(0).toInt();

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO accounts (nom, type_de_compte, solde_initial, couleur, ordre)
        VALUES (:nom, :type_de_compte, :solde_initial, :couleur, :ordre);
    )");
    query.bindValue(":nom", compte.nom());
    query.bindValue(":type_de_compte", compte.typeDeCompte());
    query.bindValue(":solde_initial", compte.soldeInitial());
    query.bindValue(":couleur", couleurOuNull(compte.couleur()));
    query.bindValue(":ordre", ordre);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    compte.setId(query.lastInsertId().toInt());
    compte.setOrdre(ordre);
    setError(errorMessage, QString());
    return true;
}

bool AccountRepository::update(const Compte &compte, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    // L'ordre n'est pas modifie ici : utiliser updateOrder()
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        UPDATE accounts
        SET nom = :nom,
            type_de_compte = :type_de_compte,
            solde_initial = :solde_initial,
            couleur = :couleur
        WHERE id = :id;
    )");
    query.bindValue(":id", compte.id());
    query.bindValue(":nom", compte.nom());
    query.bindValue(":type_de_compte", compte.typeDeCompte());
    query.bindValue(":solde_initial", compte.soldeInitial());
    query.bindValue(":couleur", couleurOuNull(compte.couleur()));

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucun compte trouve avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

bool AccountRepository::remove(int id, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM accounts WHERE id = :id;");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucun compte trouve avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

Compte AccountRepository::findById(int id, QString *errorMessage) const
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return Compte::null();
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(SELECT_COMPTES + " WHERE a.id = :id GROUP BY a.id;");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return Compte::null();
    }

    setError(errorMessage, QString());
    if (!query.next()) {
        return Compte::null();
    }

    return hydrateCompte(query);
}

QList<Compte> AccountRepository::findAll(QString *errorMessage) const
{
    QList<Compte> comptes;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return comptes;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(SELECT_COMPTES + " GROUP BY a.id ORDER BY a.ordre ASC, a.id ASC;")) {
        setError(errorMessage, query.lastError().text());
        return comptes;
    }

    while (query.next()) {
        comptes.append(hydrateCompte(query));
    }

    setError(errorMessage, QString());
    return comptes;
}

bool AccountRepository::updateOrder(const QList<int> &idsDansLOrdre, QString *errorMessage)
{
    DatabaseManager &db = DatabaseManager::instance();
    if (!db.isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    // Tout l'ordre change, ou rien : on travaille dans une transaction SQL
    if (!db.beginTransaction()) {
        setError(errorMessage, db.lastError());
        return false;
    }

    QSqlQuery query(db.database());
    query.prepare("UPDATE accounts SET ordre = :ordre WHERE id = :id;");
    for (int i = 0; i < idsDansLOrdre.size(); ++i) {
        query.bindValue(":ordre", i + 1);
        query.bindValue(":id", idsDansLOrdre.at(i));
        if (!query.exec()) {
            setError(errorMessage, query.lastError().text());
            db.rollbackTransaction();
            return false;
        }
    }

    if (!db.commitTransaction()) {
        setError(errorMessage, db.lastError());
        db.rollbackTransaction();
        return false;
    }

    setError(errorMessage, QString());
    return true;
}
