#ifndef IMPORTEXPORTSERVICE_H
#define IMPORTEXPORTSERVICE_H

#include <QDate>
#include <QList>
#include <QString>
#include <QtGlobal>

/**
 * @brief Une transaction lue dans un fichier a importer, avec son resultat de verification.
 */
struct LigneImport
{
    int numero = 0;              // position dans le fichier (pour les messages)
    QDate date;
    QString nom;
    QString description;
    qint64 montant = 0;
    QString type;                // "depense" ou "approvisionnement"
    QString compte;              // nom du compte
    QString categorie;           // nom de la categorie
    QString erreur;              // vide = ligne valide
    bool categorieInconnue = false;

    bool estValide() const { return erreur.isEmpty(); }
};

/**
 * @brief Lecture et ecriture des transactions en CSV, JSON ou XML.
 *
 * Aucun widget ici : la page ImportExportPage ne fait qu'appeler ce service.
 * Les comptes et categories sont ecrits par leur NOM (un id n'a pas de sens dans une autre base).
 */
class ImportExportService
{
public:
    enum class Format { Csv, Json, Xml };
    enum class CategorieInconnue { Creer, UtiliserAutre, Ignorer };

    // Devine le format a partir de l'extension du fichier (.csv, .json, .xml)
    static Format formatDepuisChemin(const QString &chemin);
    static QString extension(Format format);

    // Ecrit les transactions de la periode. Retourne le nombre exporte, ou -1 en cas d'erreur.
    int exporterTransactions(const QString &chemin, Format format,
                             const QDate &debut, const QDate &fin, QString *erreur) const;

    // Lit le fichier et verifie chaque ligne, SANS rien ecrire en base.
    QList<LigneImport> lireFichier(const QString &chemin, QString *erreur) const;

    // Insere les lignes valides dans une seule transaction SQL (tout ou rien).
    // Retourne le nombre importe, ou -1 en cas d'erreur.
    int importer(const QList<LigneImport> &lignes, CategorieInconnue choix, QString *erreur) const;
};

#endif // IMPORTEXPORTSERVICE_H
