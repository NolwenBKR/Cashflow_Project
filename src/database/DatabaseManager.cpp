#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QList>
#include <QVariant>

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
    if(!insererDonneesParDefaut()){
        return false;
    }
    m_lastError.clear();
    return true;
}

bool DatabaseManager::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

bool DatabaseManager::resetAllData(bool garderCategories)
{
    if (!isOpen()) {
        m_lastError = "La base de données n'est pas ouverte.";
        return false;
    }

    // Ordre important : d'abord les tables qui pointent vers les autres
    QStringList tables = {
        "transaction_etiquettes", "transactions", "budget_categories", "budgets",
        "recurrences", "cours_bourse", "actions_suivies", "accounts"
    };
    if (!garderCategories) {
        tables << "etiquettes" << "categories";
    }

    if (!beginTransaction()) {
        return false;
    }

    for (const QString &table : tables) {
        if (!executeQuery("DELETE FROM " + table + ";")) {
            rollbackTransaction();
            return false;
        }
    }

    // Remet les compteurs d'identifiants a zero pour les tables videes
    if (!executeQuery("DELETE FROM sqlite_sequence WHERE name IN ('" + tables.join("', '") + "');")) {
        rollbackTransaction();
        return false;
    }

    if (!commitTransaction()) {
        rollbackTransaction();
        return false;
    }

    // Si les categories ont ete effacees, les categories par defaut reviennent
    return insererDonneesParDefaut();
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
                nom TEXT NOT NULL,
                solde_initial INTEGER NOT NULL DEFAULT 0,
                couleur TEXT
            );
        )",
        R"(
            CREATE TABLE IF NOT EXISTS categories (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nom TEXT NOT NULL,
                couleur TEXT,
                type TEXT NOT NULL DEFAULT 'depense' CHECK (type in ('depense', 'approvisionnement'))
            );
        )",
        R"(
            CREATE TABLE IF NOT EXISTS etiquettes(
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nom TEXT NOT NULL UNIQUE,
                couleur TEXT
            );
        )",
        R"(
            CREATE TABLE IF NOT EXISTS recurrences (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nom TEXT NOT NULL,
                montant INTEGER NOT NULL,
                type TEXT NOT NULL CHECK (type IN ('depense', 'approvisionnement')),
                account_id INTEGER NOT NULL,
                category_id INTEGER NOT NULL,
                frequence TEXT NOT NULL CHECK (frequence IN ('quotidienne', 'hebdomadaire', 'mensuelle', 'annuelle')),
                intervalle INTEGER NOT NULL DEFAULT 1,
                prochaine_date DATE NOT NULL,
                date_fin DATE,
                active INTEGER NOT NULL DEFAULT 1,
                FOREIGN KEY (account_id) REFERENCES accounts(id) ON DELETE CASCADE,
                FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE RESTRICT
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
                    recurrence_id INTEGER,
                    FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE RESTRICT,
                    FOREIGN KEY (account_id) REFERENCES accounts(id) ON DELETE CASCADE,
                    FOREIGN KEY (recurrence_id) REFERENCES recurrences(id) ON DELETE SET NULL
                );
        )",
        R"(
                CREATE TABLE IF NOT EXISTS transaction_etiquettes (
                    transaction_id INTEGER NOT NULL,
                    etiquette_id INTEGER NOT NULL,
                    PRIMARY KEY (transaction_id, etiquette_id),
                    FOREIGN KEY (transaction_id) REFERENCES transactions(id) ON DELETE CASCADE,
                    FOREIGN KEY (etiquette_id) REFERENCES etiquettes(id) ON DELETE CASCADE
                );
        )",
        R"(
                CREATE TABLE IF NOT EXISTS budgets (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    nom TEXT NOT NULL,
                    plafond INTEGER NOT NULL,
                    periode TEXT NOT NULL DEFAULT 'mensuelle' CHECK (periode IN ('hebdomadaire', 'mensuelle', 'annuelle')),
                    date_debut DATE NOT NULL,
                    date_fin DATE NOT NULL,
                    seuil_alerte INTEGER NOT NULL DEFAULT 80
                );

        )",
        R"(
                CREATE TABLE IF NOT EXISTS budget_categories (
                    budget_id INTEGER NOT NULL,
                    category_id INTEGER NOT NULL,
                    PRIMARY KEY (budget_id, category_id),
                    FOREIGN KEY (budget_id) REFERENCES budgets(id) ON DELETE CASCADE,
                    FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE CASCADE
                );

        )",
        R"(
                CREATE TABLE IF NOT EXISTS actions_suivies (
                    symbole TEXT PRIMARY KEY,
                    nom TEXT,
                    ordre INTEGER NOT NULL DEFAULT 0
                );
        )",
        R"(
                CREATE TABLE IF NOT EXISTS cours_bourse (
                    symbole TEXT NOT NULL,
                    date DATE NOT NULL,
                    ouverture REAL,
                    plus_haut REAL,
                    plus_bas REAL,
                    cloture REAL NOT NULL,
                    volume INTEGER,
                    PRIMARY KEY (symbole, date),
                    FOREIGN KEY (symbole) REFERENCES actions_suivies(symbole) ON DELETE CASCADE
                );
        )",
        R"(
                CREATE INDEX IF NOT EXISTS idx_transactions_date ON transactions(date);

        )",
        R"(
                CREATE INDEX IF NOT EXISTS idx_transactions_account ON transactions(account_id);

        )"

    };

    for (const QString &sql : tables) {
        if (!executeQuery(sql)) {
            return false;
        }
    }

    return true;
}

bool DatabaseManager::insererDonneesParDefaut()
{
    // 1. Si des categories existent deja, on ne fait rien (sinon doublons a chaque lancement)
    QSqlQuery compte(m_db);
    if (!compte.exec("SELECT COUNT(*) FROM categories;") || !compte.next()) {
        m_lastError = compte.lastError().text();
        return false;
    }
    if (compte.value(0).toInt() > 0) {
        return true;
    }

    // 2. Sinon, on insere les categories par defaut : { nom, type, couleur }
    struct CategorieParDefaut { const char *nom; const char *type; const char *couleur; };
    const QList<CategorieParDefaut> categories = {
        { "Alimentation",  "depense",           "#0F766E" },
        { "Logement",      "depense",           "#1D4ED8" },
        { "Transport",     "depense",           "#D97706" },
        { "Factures",      "depense",           "#7C3AED" },
        { "Communication", "depense",           "#DB2777" },
        { "Loisirs",       "depense",           "#65A30D" },
        { "Santé",         "depense",           "#DC2626" },
        { "Autre",         "depense",           "#5B6B6A" },
        { "Salaire",       "approvisionnement", "#047857" },
        { "Freelance",     "approvisionnement", "#0891B2" },
        { "Cadeaux reçus", "approvisionnement", "#9333EA" }
    };

    if (!beginTransaction()) {
        return false;
    }

    QSqlQuery insertion(m_db);
    insertion.prepare("INSERT INTO categories (nom, type, couleur) VALUES (:nom, :type, :couleur);");
    for (const CategorieParDefaut &c : categories) {
        insertion.bindValue(":nom", QString::fromUtf8(c.nom));
        insertion.bindValue(":type", QString::fromUtf8(c.type));
        insertion.bindValue(":couleur", QString::fromUtf8(c.couleur));
        if (!insertion.exec()) {
            m_lastError = insertion.lastError().text();
            rollbackTransaction();
            return false;
        }
    }

    return commitTransaction();
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
