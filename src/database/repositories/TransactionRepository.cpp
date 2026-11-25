#include "TransactionRepository.h"
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

Transaction hydrateTransaction(const QSqlQuery &query)
{
    const QDate date = QDate::fromString(query.value(1).toString(), Qt::ISODate);
    const QString timeStr = query.value(2).toString();
    const QTime time = timeStr.isEmpty() ? QTime() : QTime::fromString(timeStr, "HH:mm");
    const qint64 montant = qMax<qint64>(0, query.value(5).toLongLong());
    const Transaction::Type type = Transaction::typeFromString(query.value(8).toString());

    return Transaction(query.value(0).toInt(),
                       date,
                       time,
                       query.value(3).toString(),
                       query.value(4).toString(),
                       montant,
                       query.value(6).toInt(),
                       query.value(7).toInt(),
                       type,
                       query.value(9).toInt());
}
} // namespace

bool TransactionRepository::create(Transaction &transaction, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO transactions (date, time, nom, description, montant, category_id, account_id, type, recurrence_id)
        VALUES (:date, :time, :nom, :description, :montant, :category_id, :account_id, :type, :recurrence_id);
    )");
    query.bindValue(":date", transaction.date().toString(Qt::ISODate));
    query.bindValue(":time", transaction.time().isValid() ? QVariant(transaction.time().toString("HH:mm")) : QVariant());
    query.bindValue(":nom", transaction.nom());
    query.bindValue(":description", transaction.description().isEmpty() ? QVariant() : QVariant(transaction.description()));
    query.bindValue(":montant", transaction.montant());
    query.bindValue(":category_id", transaction.categoryId());
    query.bindValue(":account_id", transaction.accountId());
        query.bindValue(":recurrence_id", transaction.recurrenceId() > 0 ? QVariant(transaction.recurrenceId()) : QVariant());

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    transaction.setId(query.lastInsertId().toInt());
    setError(errorMessage, QString());
    return true;
}

bool TransactionRepository::update(const Transaction &transaction, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        UPDATE transactions
        SET date = :date,
            time = :time,
            nom = :nom,
            description = :description,
            montant = :montant,
            category_id = :category_id,
            account_id = :account_id,
            type = :type,
            recurrence_id = :recurrence_id
        WHERE id = :id;
    )");
    query.bindValue(":id", transaction.id());
    query.bindValue(":date", transaction.date().toString(Qt::ISODate));
    query.bindValue(":time", transaction.time().isValid() ? QVariant(transaction.time().toString("HH:mm")) : QVariant());
    query.bindValue(":nom", transaction.nom());
    query.bindValue(":description", transaction.description().isEmpty() ? QVariant() : QVariant(transaction.description()));
    query.bindValue(":montant", transaction.montant());
    query.bindValue(":category_id", transaction.categoryId());
    query.bindValue(":account_id", transaction.accountId());
    query.bindValue(":recurrence_id", transaction.recurrenceId() > 0 ? QVariant(transaction.recurrenceId()) : QVariant());

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucune transaction trouvee avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

bool TransactionRepository::remove(int id, QString *errorMessage)
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM transactions WHERE id = :id;");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() <= 0) {
        setError(errorMessage, "Aucune transaction trouvee avec cet identifiant.");
        return false;
    }

    setError(errorMessage, QString());
    return true;
}

Transaction TransactionRepository::findById(int id, QString *errorMessage) const
{
    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return Transaction::null();
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, date, time, nom, description, montant, category_id, account_id, type, recurrence_id
        FROM transactions
        WHERE id = :id;
    )");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return Transaction::null();
    }

    setError(errorMessage, QString());
    if (!query.next()) {
        return Transaction::null();
    }

    return hydrateTransaction(query);
}

QList<Transaction> TransactionRepository::findAll(QString *errorMessage) const
{
    QList<Transaction> items;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return items;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(R"(
        SELECT id, date, time, nom, description, montant, category_id, account_id, type,recurrence_id
        FROM transactions
        ORDER BY date DESC, id DESC;
    )")) {
        setError(errorMessage, query.lastError().text());
        return items;
    }

    while (query.next()) {
        items.append(hydrateTransaction(query));
    }

    setError(errorMessage, QString());
    return items;
}

QList<Transaction> TransactionRepository::findByAccountId(int accountId, QString *errorMessage) const
{
    QList<Transaction> items;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return items;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, date, time, nom, description, montant, category_id, account_id, type, recurrence_id
        FROM transactions
        WHERE account_id = :account_id
        ORDER BY date DESC, id DESC;
    )");
    query.bindValue(":account_id", accountId);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return items;
    }

    while (query.next()) {
        items.append(hydrateTransaction(query));
    }

    setError(errorMessage, QString());
    return items;
}

QList<Transaction> TransactionRepository::findByCategoryId(int categoryId, QString *errorMessage) const
{
    QList<Transaction> items;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return items;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, date, time, nom, description, montant, category_id, account_id, type,recurrence_id
        FROM transactions
        WHERE category_id = :category_id
        ORDER BY date DESC, id DESC;
    )");
    query.bindValue(":category_id", categoryId);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return items;
    }

    while (query.next()) {
        items.append(hydrateTransaction(query));
    }

    setError(errorMessage, QString());
    return items;
}

QList<Transaction> TransactionRepository::findByDateRange(const QDate &startDate,
                                                             const QDate &endDate,
                                                             QString *errorMessage) const
{
    QList<Transaction> items;

    if (!DatabaseManager::instance().isOpen()) {
        setError(errorMessage, "La base de donnees n'est pas ouverte.");
        return items;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, date, time, nom, description, montant, category_id, account_id, type, recurrence_id
        FROM transactions
        WHERE date BETWEEN :start_date AND :end_date
        ORDER BY date DESC, id DESC;
    )");
    query.bindValue(":start_date", startDate.toString(Qt::ISODate));
    query.bindValue(":end_date", endDate.toString(Qt::ISODate));

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return items;
    }

    while (query.next()) {
        items.append(hydrateTransaction(query));
    }

    setError(errorMessage, QString());
    return items;
}
