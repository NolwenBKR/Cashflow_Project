#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

/**
 * @brief Gère l'instance unique de la base de données SQLite de l'application.
 *
 * Centralise l'initialisation de la connexion, la création des tables,
 * les transactions SQL et la réinitialisation des données.
 */
class DatabaseManager
{
public:
    /**
     * @brief Retourne l'instance unique partagée de DatabaseManager.
     * @return Référence vers l'instance globale.
     */
    static DatabaseManager &instance();

    // Empêche la copie de l'instance unique (passage par valeur ou construction par copie)
    DatabaseManager(const DatabaseManager &) = delete;
    // Empêche l'affectation d'une instance vers une autre (ex: DatabaseManager x = y)
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    /**
     * @brief Initialise la base de données, ouvre la connexion et crée la base
     * de donnee et les tables si nécessaire.
     * On utilise la connexion par défaut de Qt puisqu'une seule base est utilisée.
     * Ca permet de creer des QSqlQuery sans avoir besoin de renseigner la base de donnee.
     * @param dbPath Chemin vers le fichier de base de données.
     * @return true si l'initialisation réussit.
     */
    bool initDatabase(const QString &dbPath = "cashflood.db");

    /**
     * @brief Utile pour verifier plus tard si la connexion à la base de données
     * est actuellement ouverte.
     * @return true si la base est valide et ouverte.
     */
    bool isOpen() const;

    /**
     * @brief Reinitialise le schema de la base de donnees
     * @return true si le reset réussit.
     */
    bool resetAllData(bool gargerCategories = true);

    /**
     * @brief Démarre une transaction SQL.
     * @note Toujours valider (commit) ou annuler (rollback) une transaction ouverte.
     * @return true si la transaction a démarré.
     */
    bool beginTransaction();

    /**
     * @brief Valider et appliquer les modifications de la transaction en cours.
     * @return true si le commit réussit.
     */
    bool commitTransaction();

    /**
     * @brief Annuler les modifications de la transaction en cours.
     * @return true si le rollback réussit.
     */
    bool rollbackTransaction();

    /**
     * @brief Retourne l'objet de connexion QSqlDatabase sous-jacent.
     * Bien qu'on utilise la connection par defaut et donc la db n'a pas besoin
     * d'etre explicitement renseigne pour les queries, ca pourrait servir.
     * @return Instance QSqlDatabase associée.
     */
    QSqlDatabase database() const;

    /**
     * @brief Retourne le dernier message d'erreur SQL ou technique rencontré.
     * @return Description de la dernière erreur.
     */
    QString lastError() const;

private:
    // Constructeur privé pour imposer l'accès via instance()
    DatabaseManager() = default;
    // Destructeur privé : ferme proprement la base de données si elle est ouverte
    ~DatabaseManager();

    /**
     * @brief Crée les tables de l'application si elles n'existent pas encore.
     * @note A appeller l'initialisation de la base de donnee.
     * @return true si toutes les requêtes de création réussissent, false sinon.
     */
    bool createTables();
    bool insererDonneeParDefaut();

    // Exécute une requête SQL brute interne et met à jour lastError en cas d'échec.
    bool executeQuery(const QString &sql);

    QSqlDatabase m_db;      ///< Connexion à la base de données SQLite
    QString m_lastError;    ///< Dernier message d'erreur enregistré
};

#endif // DATABASEMANAGER_H
