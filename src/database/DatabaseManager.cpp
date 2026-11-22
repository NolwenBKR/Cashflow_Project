#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initDatabase(const QString &dbPath)
{
    // On recupere la connection par defaur si elle est deja ouverte.
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        m_db = QSqlDatabase::database();
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        return false;
    }

    // On a besoin d'active manuellement les cles etrangeres cote SQLite
    if (!executeQuery("PRAGMA foreign_keys = ON;")) {
        return false;
    }

    if (!createTables()) {
        return false;
    }

    m_lastError.clear();
    return true;
}

bool DatabaseManager::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

bool DatabaseManager::resetAllData()
{
    if (!isOpen()) {
        m_lastError = "La base de données n'est pas ouverte.";
        return false;
    }

    if (!beginTransaction()) {
        return false;
    }

    const QStringList statements = {
        "DELETE FROM transactions;",
        "DELETE FROM budgets;",
        "DELETE FROM accounts;",
        "DELETE FROM categories;",
        "DELETE FROM sqlite_sequence WHERE name IN ('transactions', 'budgets', 'accounts', 'categories');"
    };

    for (const QString &sql : statements) {
        if (!executeQuery(sql)) {
            rollbackTransaction();
            return false;
        }
    }

    if (!commitTransaction()) {
        rollbackTransaction();
        return false;
    }

    m_lastError.clear();
    return true;
}

bool DatabaseManager::beginTransaction()
{
    if (!isOpen()) {
        m_lastError = "La base de données n'est pas ouverte.";
        return false;
    }

    if (!m_db.transaction()) {
        m_lastError = m_db.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}

bool DatabaseManager::commitTransaction()
{
    if (!isOpen()) {
        m_lastError = "La base de données n'est pas ouverte.";
        return false;
    }

    if (!m_db.commit()) {
        m_lastError = m_db.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}

bool DatabaseManager::rollbackTransaction()
{
    if (!isOpen()) {
        return false;
    }

    if (!m_db.rollback()) {
        m_lastError = m_db.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}

QSqlDatabase DatabaseManager::database() const
{
    return m_db;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::createTables()
{
    // Ordre respectant les clés étrangères
    const QStringList tables = {
        R"(
            CREATE TABLE IF NOT EXISTS accounts (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nom VARCHAR(255) NOT NULL,
                solde INTEGER NOT NULL DEFAULT 0,
                type_de_compte TEXT NOT NULL
            );
        )",
        R"(
            CREATE TABLE IF NOT EXISTS categories (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nom VARCHAR(255) NOT NULL,
                couleur VARCHAR(7) NOT NULL
            );
        )",
        R"(
            CREATE TABLE IF NOT EXISTS budgets (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                plafond INTEGER NOT NULL,
                avancement INTEGER NOT NULL DEFAULT 0,
                category_id INTEGER NOT NULL,
                date_debut DATE NOT NULL,
                date_fin DATE NOT NULL,
                FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE CASCADE
            );
        )",
        R"(
            CREATE TABLE IF NOT EXISTS transactions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                date DATE NOT NULL,
                time VARCHAR(5),
                nom VARCHAR(255) NOT NULL,
                description TEXT,
                montant INTEGER NOT NULL,
                category_id INTEGER NOT NULL,
                account_id INTEGER NOT NULL,
                type VARCHAR(20) NOT NULL CHECK (type IN ('depense', 'approvisionnement')),
                FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE RESTRICT,
                FOREIGN KEY (account_id) REFERENCES accounts(id) ON DELETE CASCADE
            );
        )"
    };

    for (const QString &sql : tables) {
        if (!executeQuery(sql)) {
            return false;
        }
    }

    return true;
}

bool DatabaseManager::executeQuery(const QString &sql)
{
    if (!isOpen()) {
        m_lastError = "La base de données n'est pas ouverte.";
        return false;
    }

    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}
