#include "ImportExportService.h"

#include "../../database/DatabaseManager.h"
#include "../../database/repositories/AccountRepository.h"
#include "../../database/repositories/CategoryRepository.h"
#include "../../database/repositories/TransactionRepository.h"

#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace {

void definirErreur(QString *erreur, const QString &message)
{
    if (erreur) {
        *erreur = message;
    }
}

// --- CSV : separateur point-virgule (celui qu'Excel attend en francais) ---

QString champCsv(QString texte)
{
    texte.replace(QLatin1Char('\n'), QLatin1Char(' ')).replace(QLatin1Char('\r'), QLatin1Char(' '));
    if (texte.contains(QLatin1Char(';')) || texte.contains(QLatin1Char('"'))) {
        texte.replace(QStringLiteral("\""), QStringLiteral("\"\""));   // " devient ""
        return QLatin1Char('"') + texte + QLatin1Char('"');
    }
    return texte;
}

QStringList decouperLigneCsv(const QString &ligne)
{
    QStringList champs;
    QString courant;
    bool entreGuillemets = false;
    for (int i = 0; i < ligne.size(); ++i) {
        const QChar c = ligne.at(i);
        if (entreGuillemets) {
            if (c == QLatin1Char('"')) {
                if (i + 1 < ligne.size() && ligne.at(i + 1) == QLatin1Char('"')) {
                    courant += QLatin1Char('"');   // "" = un guillemet dans le texte
                    ++i;
                } else {
                    entreGuillemets = false;
                }
            } else {
                courant += c;
            }
        } else if (c == QLatin1Char('"')) {
            entreGuillemets = true;
        } else if (c == QLatin1Char(';')) {
            champs << courant;
            courant.clear();
        } else {
            courant += c;
        }
    }
    champs << courant;
    return champs;
}

// --- Verification d'une ligne lue (formats CSV, JSON et XML) ---

void verifierLigne(LigneImport &ligne, const QString &dateTexte, const QString &montantTexte,
                   const QHash<QString, int> &comptes, const QHash<QString, int> &categories)
{
    ligne.date = QDate::fromString(dateTexte.trimmed(), Qt::ISODate);
    bool montantOk = false;
    ligne.montant = montantTexte.trimmed().toLongLong(&montantOk);
    ligne.type = ligne.type.trimmed().toLower();

    if (!ligne.date.isValid()) {
        ligne.erreur = QObject::tr("Date invalide : « %1 » (format attendu AAAA-MM-JJ)").arg(dateTexte);
    } else if (ligne.nom.trimmed().isEmpty()) {
        ligne.erreur = QObject::tr("Nom vide");
    } else if (!montantOk || ligne.montant <= 0) {
        ligne.erreur = QObject::tr("Montant invalide : « %1 »").arg(montantTexte);
    } else if (ligne.type != QLatin1String("depense") && ligne.type != QLatin1String("approvisionnement")) {
        ligne.erreur = QObject::tr("Type invalide : « %1 »").arg(ligne.type);
    } else if (!comptes.contains(ligne.compte.trimmed().toLower())) {
        ligne.erreur = QObject::tr("Compte inconnu : « %1 » (créez-le d'abord)").arg(ligne.compte);
    }
    ligne.categorieInconnue = !categories.contains(ligne.categorie.trimmed().toLower());
}

} // namespace

ImportExportService::Format ImportExportService::formatDepuisChemin(const QString &chemin)
{
    const QString suffixe = QFileInfo(chemin).suffix().toLower();
    if (suffixe == QLatin1String("json")) {
        return Format::Json;
    }
    if (suffixe == QLatin1String("xml")) {
        return Format::Xml;
    }
    return Format::Csv;
}

QString ImportExportService::extension(Format format)
{
    switch (format) {
    case Format::Json: return QStringLiteral("json");
    case Format::Xml:  return QStringLiteral("xml");
    case Format::Csv:
    default:           return QStringLiteral("csv");
    }
}

int ImportExportService::exporterTransactions(const QString &chemin, Format format,
                                              const QDate &debut, const QDate &fin,
                                              QString *erreur) const
{
    // 1. Lire les donnees : transactions de la periode, et les noms des comptes et categories
    QString erreurBase;
    TransactionRepository transactions;
    const QList<Transaction> liste = transactions.findByDateRange(debut, fin, &erreurBase);
    if (!erreurBase.isEmpty()) {
        definirErreur(erreur, erreurBase);
        return -1;
    }

    QHash<int, QString> nomsComptes;
    for (const Compte &c : AccountRepository().findAll()) {
        nomsComptes.insert(c.id(), c.nom());
    }
    QHash<int, QString> nomsCategories;
    for (const Category &c : CategoryRepository().findAll()) {
        nomsCategories.insert(c.id(), c.nom());
    }

    // 2. Ouvrir le fichier
    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        definirErreur(erreur, QObject::tr("Impossible d'écrire le fichier : %1").arg(fichier.errorString()));
        return -1;
    }

    // 3. Ecrire dans le format choisi
    if (format == Format::Csv) {
        QTextStream flux(&fichier);
        flux.setCodec("UTF-8");                  // sinon les accents sont abimes (Qt 5)
        flux.setGenerateByteOrderMark(true);     // pour qu'Excel reconnaisse l'UTF-8
        flux << "date;nom;description;montant;type;compte;categorie\n";
        for (const Transaction &t : liste) {
            flux << t.date().toString(Qt::ISODate) << ';'
                 << champCsv(t.nom()) << ';'
                 << champCsv(t.description()) << ';'
                 << t.montant() << ';'
                 << Transaction::typeToString(t.type()) << ';'
                 << champCsv(nomsComptes.value(t.accountId())) << ';'
                 << champCsv(nomsCategories.value(t.categoryId())) << '\n';
        }
    } else if (format == Format::Json) {
        QJsonArray tableau;
        for (const Transaction &t : liste) {
            QJsonObject objet;
            objet.insert(QStringLiteral("date"), t.date().toString(Qt::ISODate));
            objet.insert(QStringLiteral("nom"), t.nom());
            objet.insert(QStringLiteral("description"), t.description());
            objet.insert(QStringLiteral("montant"), static_cast<double>(t.montant()));
            objet.insert(QStringLiteral("type"), Transaction::typeToString(t.type()));
            objet.insert(QStringLiteral("compte"), nomsComptes.value(t.accountId()));
            objet.insert(QStringLiteral("categorie"), nomsCategories.value(t.categoryId()));
            tableau.append(objet);
        }
        QJsonObject racine;
        racine.insert(QStringLiteral("version"), 1);
        racine.insert(QStringLiteral("transactions"), tableau);
        fichier.write(QJsonDocument(racine).toJson(QJsonDocument::Indented));
    } else {
        QXmlStreamWriter xml(&fichier);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement(QStringLiteral("cashflood"));
        xml.writeAttribute(QStringLiteral("version"), QStringLiteral("1"));
        for (const Transaction &t : liste) {
            xml.writeStartElement(QStringLiteral("transaction"));
            xml.writeAttribute(QStringLiteral("date"), t.date().toString(Qt::ISODate));
            xml.writeAttribute(QStringLiteral("type"), Transaction::typeToString(t.type()));
            xml.writeAttribute(QStringLiteral("montant"), QString::number(t.montant()));
            xml.writeTextElement(QStringLiteral("nom"), t.nom());
            xml.writeTextElement(QStringLiteral("description"), t.description());
            xml.writeTextElement(QStringLiteral("compte"), nomsComptes.value(t.accountId()));
            xml.writeTextElement(QStringLiteral("categorie"), nomsCategories.value(t.categoryId()));
            xml.writeEndElement();
        }
        xml.writeEndElement();
        xml.writeEndDocument();
    }

    definirErreur(erreur, QString());
    return liste.size();
}

QList<LigneImport> ImportExportService::lireFichier(const QString &chemin, QString *erreur) const
{
    QList<LigneImport> lignes;

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::ReadOnly)) {
        definirErreur(erreur, QObject::tr("Impossible d'ouvrir le fichier : %1").arg(fichier.errorString()));
        return lignes;
    }

    // Noms connus, en minuscules, pour verifier chaque ligne
    QHash<QString, int> comptes;
    for (const Compte &c : AccountRepository().findAll()) {
        comptes.insert(c.nom().trimmed().toLower(), c.id());
    }
    QHash<QString, int> categories;
    for (const Category &c : CategoryRepository().findAll()) {
        categories.insert(c.nom().trimmed().toLower(), c.id());
    }

    const Format format = formatDepuisChemin(chemin);

    if (format == Format::Csv) {
        QTextStream flux(&fichier);
        flux.setCodec("UTF-8");
        int numero = 0;
        bool premiereLigne = true;
        while (!flux.atEnd()) {
            const QString texte = flux.readLine();
            ++numero;
            if (premiereLigne) {           // la ligne d'en-tete
                premiereLigne = false;
                continue;
            }
            if (texte.trimmed().isEmpty()) {
                continue;
            }
            const QStringList champs = decouperLigneCsv(texte);
            LigneImport ligne;
            ligne.numero = numero;
            if (champs.size() < 7) {
                ligne.erreur = QObject::tr("Ligne incomplète : %1 colonnes au lieu de 7").arg(champs.size());
                lignes << ligne;
                continue;
            }
            ligne.nom = champs.at(1);
            ligne.description = champs.at(2);
            ligne.type = champs.at(4);
            ligne.compte = champs.at(5);
            ligne.categorie = champs.at(6);
            verifierLigne(ligne, champs.at(0), champs.at(3), comptes, categories);
            lignes << ligne;
        }
    } else if (format == Format::Json) {
        QJsonParseError erreurJson;
        const QJsonDocument document = QJsonDocument::fromJson(fichier.readAll(), &erreurJson);
        if (erreurJson.error != QJsonParseError::NoError || !document.isObject()) {
            definirErreur(erreur, QObject::tr("Fichier JSON invalide : %1").arg(erreurJson.errorString()));
            return lignes;
        }
        const QJsonArray tableau = document.object().value(QStringLiteral("transactions")).toArray();
        for (int i = 0; i < tableau.size(); ++i) {
            const QJsonObject objet = tableau.at(i).toObject();
            LigneImport ligne;
            ligne.numero = i + 1;
            ligne.nom = objet.value(QStringLiteral("nom")).toString();
            ligne.description = objet.value(QStringLiteral("description")).toString();
            ligne.type = objet.value(QStringLiteral("type")).toString();
            ligne.compte = objet.value(QStringLiteral("compte")).toString();
            ligne.categorie = objet.value(QStringLiteral("categorie")).toString();
            const QJsonValue montant = objet.value(QStringLiteral("montant"));
            const QString montantTexte = montant.isDouble() ? QString::number(static_cast<qint64>(montant.toDouble()))
                                                            : montant.toString();
            verifierLigne(ligne, objet.value(QStringLiteral("date")).toString(), montantTexte,
                          comptes, categories);
            lignes << ligne;
        }
    } else {
        QXmlStreamReader xml(&fichier);
        int numero = 0;
        while (!xml.atEnd()) {
            xml.readNext();
            if (!xml.isStartElement() || xml.name() != QLatin1String("transaction")) {
                continue;
            }
            LigneImport ligne;
            ligne.numero = ++numero;
            const QString dateTexte = xml.attributes().value(QStringLiteral("date")).toString();
            const QString montantTexte = xml.attributes().value(QStringLiteral("montant")).toString();
            ligne.type = xml.attributes().value(QStringLiteral("type")).toString();
            // Lire les sous-elements jusqu'a </transaction>
            while (xml.readNextStartElement()) {
                const QString balise = xml.name().toString();
                const QString valeur = xml.readElementText();
                if (balise == QLatin1String("nom")) ligne.nom = valeur;
                else if (balise == QLatin1String("description")) ligne.description = valeur;
                else if (balise == QLatin1String("compte")) ligne.compte = valeur;
                else if (balise == QLatin1String("categorie")) ligne.categorie = valeur;
            }
            verifierLigne(ligne, dateTexte, montantTexte, comptes, categories);
            lignes << ligne;
        }
        if (xml.hasError()) {
            definirErreur(erreur, QObject::tr("Fichier XML invalide (ligne %1) : %2")
                              .arg(xml.lineNumber()).arg(xml.errorString()));
            return QList<LigneImport>();
        }
    }

    definirErreur(erreur, QString());
    return lignes;
}

int ImportExportService::importer(const QList<LigneImport> &lignes, CategorieInconnue choix,
                                  QString *erreur) const
{
    AccountRepository comptesRepo;
    CategoryRepository categoriesRepo;
    TransactionRepository transactionsRepo;

    QHash<QString, int> comptes;
    for (const Compte &c : comptesRepo.findAll()) {
        comptes.insert(c.nom().trimmed().toLower(), c.id());
    }
    QHash<QString, int> categories;
    for (const Category &c : categoriesRepo.findAll()) {
        categories.insert(c.nom().trimmed().toLower(), c.id());
    }

    // Tout ou rien : une seule transaction SQL pour tout l'import
    DatabaseManager &base = DatabaseManager::instance();
    if (!base.beginTransaction()) {
        definirErreur(erreur, base.lastError());
        return -1;
    }

    int importees = 0;
    for (const LigneImport &ligne : lignes) {
        if (!ligne.estValide()) {
            continue;
        }

        // Trouver (ou creer) la categorie
        QString cleCategorie = ligne.categorie.trimmed().toLower();
        if (!categories.contains(cleCategorie)) {
            if (choix == CategorieInconnue::Ignorer) {
                continue;
            }
            Category nouvelle;
            if (choix == CategorieInconnue::UtiliserAutre) {
                cleCategorie = QStringLiteral("autre");
                nouvelle = Category(0, QStringLiteral("Autre"), QStringLiteral("#5B6B6A"), QStringLiteral("depense"));
            } else {
                nouvelle = Category(0, ligne.categorie.trimmed(), QStringLiteral("#5B6B6A"), ligne.type);
            }
            if (!categories.contains(cleCategorie)) {
                QString erreurCategorie;
                if (!categoriesRepo.create(nouvelle, &erreurCategorie)) {
                    base.rollbackTransaction();
                    definirErreur(erreur, QObject::tr("Ligne %1 : %2").arg(ligne.numero).arg(erreurCategorie));
                    return -1;
                }
                categories.insert(cleCategorie, nouvelle.id());
            }
        }

        bool typeOk = false;
        Transaction transaction(0, ligne.date, QTime(), ligne.nom.trimmed(), ligne.description,
                                ligne.montant, categories.value(cleCategorie),
                                comptes.value(ligne.compte.trimmed().toLower()),
                                Transaction::typeFromString(ligne.type, &typeOk));

        QString erreurTransaction;
        if (!transactionsRepo.create(transaction, &erreurTransaction)) {
            base.rollbackTransaction();
            definirErreur(erreur, QObject::tr("Ligne %1 : %2").arg(ligne.numero).arg(erreurTransaction));
            return -1;
        }
        ++importees;
    }

    if (!base.commitTransaction()) {
        base.rollbackTransaction();
        definirErreur(erreur, base.lastError());
        return -1;
    }

    definirErreur(erreur, QString());
    return importees;
}
