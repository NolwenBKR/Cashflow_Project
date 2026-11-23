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
} // namespace

bool AccountRepository::create(Compte &compte, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO accounts (nom, solde, solde_initial, type_de_compte)
        VALUES (:nom, :solde, :initial_solde, :type_de_compte);
    )");
    query.bindValue(":nom", compte.nom());
    query.bindValue(":solde", compte.solde());
    query.bindValue(":solde_initial", compte.soldeInitial());
    query.bindValue(":type_de_compte", compte.typeDeCompte());

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    compte.setId(query.lastInsertId().toInt());
    setError(errorMessage, QString());
    return true;
}

bool AccountRepository::update(const Compte &compte, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        UPDATE accounts
        SET nom = :nom,
            solde = :solde,
            solde_initial = :solde_initial,
            type_de_compte = :type_de_compte
        WHERE id = :id;
    )");
    query.bindValue(":id", compte.id());
    query.bindValue(":nom", compte.nom());
    query.bindValue(":solde", compte.solde());
    query.bindValue(":solde_initial", compte.soldeInitial());
    query.bindValue(":type_de_compte", compte.typeDeCompte());

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
    query.prepare(R"(
        SELECT id, nom, solde, solde_initial, type_de_compte
        FROM accounts
        WHERE id = :id;
    )");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return Compte::null();
    }

    setError(errorMessage, QString());
    if (!query.next()) {
        return Compte::null();
    }

    return Compte(query.value(0).toInt(),
                  query.value(1).toString(),
                  query.value(2).toInt(),
                  query.value(3).toInt(),
                  query.value(4).toString());
}

QList<Compte> AccountRepository::findAll(QString *errorMessage) const
{
    QList<Compte> comptes;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return comptes;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec("SELECT id, nom, solde, solde_initial, type_de_compte FROM accounts ORDER BY id ASC;")) {
        setError(errorMessage, query.lastError().text());
        return comptes;
    }

    while (query.next()) {
        comptes.append(Compte(query.value(0).toInt(),
                              query.value(1).toString(),
                              query.value(2).toInt(),
                              query.value(3).toInt(),
                              query.value(4).toString()));
    }

    setError(errorMessage, QString());
    return comptes;
}

bool AccountRepository::updateBalance(int accountId, int newBalance, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE accounts SET solde = :solde WHERE id = :id;");
    query.bindValue(":id", accountId);
    query.bindValue(":solde", newBalance);

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
