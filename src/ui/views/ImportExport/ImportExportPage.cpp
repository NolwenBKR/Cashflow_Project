#include "ImportExportPage.h"

#include "../../utils/Formatage.h"
#include "../../../core/services/DataNotifier.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

ImportExportPage::ImportExportPage(QWidget *parent)
    : QWidget(parent)
{
    auto *titre = new QLabel(tr("Import / Export"), this);
    titre->setStyleSheet(QStringLiteral("font-size: 24px; font-weight: 700;"));

    // ---------- Bloc Export ----------
    auto *blocExport = new QGroupBox(tr("Exporter les transactions"), this);
    m_formatExport = new QComboBox(blocExport);
    m_formatExport->addItem(tr("CSV (tableur, Excel)"), static_cast<int>(ImportExportService::Format::Csv));
    m_formatExport->addItem(tr("JSON"), static_cast<int>(ImportExportService::Format::Json));
    m_formatExport->addItem(tr("XML"), static_cast<int>(ImportExportService::Format::Xml));

    const QDate aujourdhui = QDate::currentDate();
    m_debut = new QDateEdit(QDate(aujourdhui.year(), 1, 1), blocExport);
    m_fin = new QDateEdit(aujourdhui, blocExport);
    for (QDateEdit *champ : {m_debut, m_fin}) {
        champ->setCalendarPopup(true);
        champ->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    }

    auto *boutonExporter = new QPushButton(tr("Exporter…"), blocExport);

    auto *formulaireExport = new QFormLayout(blocExport);
    formulaireExport->addRow(tr("Format"), m_formatExport);
    formulaireExport->addRow(tr("Du"), m_debut);
    formulaireExport->addRow(tr("Au"), m_fin);
    formulaireExport->addRow(boutonExporter);

    // ---------- Bloc Import ----------
    auto *blocImport = new QGroupBox(tr("Importer un fichier"), this);
    auto *boutonChoisir = new QPushButton(tr("Choisir un fichier…"), blocImport);
    m_fichierChoisi = new QLabel(tr("Aucun fichier choisi (CSV, JSON ou XML)."), blocImport);
    m_fichierChoisi->setWordWrap(true);

    m_apercu = new QTableWidget(0, 8, blocImport);
    m_apercu->setHorizontalHeaderLabels({tr("Statut"), tr("Ligne"), tr("Date"), tr("Nom"),
                                         tr("Montant"), tr("Type"), tr("Compte"), tr("Catégorie")});
    m_apercu->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_apercu->horizontalHeader()->setStretchLastSection(true);
    m_apercu->verticalHeader()->setVisible(false);
    m_apercu->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_choixCategorie = new QComboBox(blocImport);
    m_choixCategorie->addItem(tr("La créer automatiquement"),
                              static_cast<int>(ImportExportService::CategorieInconnue::Creer));
    m_choixCategorie->addItem(tr("Utiliser « Autre »"),
                              static_cast<int>(ImportExportService::CategorieInconnue::UtiliserAutre));
    m_choixCategorie->addItem(tr("Ignorer la ligne"),
                              static_cast<int>(ImportExportService::CategorieInconnue::Ignorer));

    m_boutonImporter = new QPushButton(tr("Importer"), blocImport);
    m_boutonImporter->setEnabled(false);

    auto *ligneChoix = new QHBoxLayout;
    ligneChoix->addWidget(new QLabel(tr("Si une catégorie est inconnue :"), blocImport));
    ligneChoix->addWidget(m_choixCategorie);
    ligneChoix->addStretch();
    ligneChoix->addWidget(m_boutonImporter);

    auto *dispositionImport = new QVBoxLayout(blocImport);
    dispositionImport->addWidget(boutonChoisir, 0, Qt::AlignLeft);
    dispositionImport->addWidget(m_fichierChoisi);
    dispositionImport->addWidget(m_apercu);
    dispositionImport->addLayout(ligneChoix);

    // ---------- Page ----------
    auto *colonnes = new QHBoxLayout;
    colonnes->addWidget(blocExport, 1, Qt::AlignTop);
    colonnes->addWidget(blocImport, 2);

    auto *page = new QVBoxLayout(this);
    page->addWidget(titre);
    page->addLayout(colonnes);

    connect(boutonExporter, &QPushButton::clicked, this, &ImportExportPage::exporter);
    connect(boutonChoisir, &QPushButton::clicked, this, &ImportExportPage::choisirFichierAImporter);
    connect(m_boutonImporter, &QPushButton::clicked, this, &ImportExportPage::importer);
    connect(m_choixCategorie, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ImportExportPage::afficherApercu);
}

void ImportExportPage::exporter()
{
    if (m_debut->date() > m_fin->date()) {
        QMessageBox::warning(this, tr("Période invalide"), tr("La date de début est après la date de fin."));
        return;
    }

    const auto format = static_cast<ImportExportService::Format>(m_formatExport->currentData().toInt());
    const QString extension = ImportExportService::extension(format);
    const QString nomPropose = QDir::homePath() + QStringLiteral("/transactions_%1.%2")
                                                      .arg(QDate::currentDate().toString(Qt::ISODate), extension);

    const QString chemin = QFileDialog::getSaveFileName(
        this, tr("Exporter les transactions"), nomPropose,
        tr("Fichier %1 (*.%2)").arg(extension.toUpper(), extension));
    if (chemin.isEmpty()) {
        return;   // l'utilisateur a annule
    }

    QString erreur;
    const int nombre = m_service.exporterTransactions(chemin, format, m_debut->date(), m_fin->date(), &erreur);
    if (nombre < 0) {
        QMessageBox::critical(this, tr("Erreur"), erreur);
        return;
    }

    QMessageBox::information(this, tr("Export terminé"),
                             tr("%n transaction(s) exportée(s) dans :\n%1", nullptr, nombre).arg(chemin));
}

void ImportExportPage::choisirFichierAImporter()
{
    const QString chemin = QFileDialog::getOpenFileName(
        this, tr("Importer des transactions"), QDir::homePath(),
        tr("Fichiers de données (*.csv *.json *.xml)"));
    if (chemin.isEmpty()) {
        return;
    }

    QString erreur;
    m_lignes = m_service.lireFichier(chemin, &erreur);
    if (!erreur.isEmpty()) {
        m_lignes.clear();
        QMessageBox::critical(this, tr("Fichier illisible"), erreur);
    }

    m_fichierChoisi->setText(tr("%1 · %n ligne(s) lue(s)", nullptr, m_lignes.size())
                                 .arg(QFileInfo(chemin).fileName()));
    afficherApercu();
}

void ImportExportPage::afficherApercu()
{
    const auto choix = choixCategorieInconnue();
    m_apercu->setRowCount(m_lignes.size());

    int aImporter = 0;
    for (int i = 0; i < m_lignes.size(); ++i) {
        const LigneImport &ligne = m_lignes.at(i);

        QString statut;
        QColor couleur;
        if (!ligne.estValide()) {
            statut = tr("Erreur : %1").arg(ligne.erreur);
            couleur = QColor(QStringLiteral("#B42318"));
        } else if (ligne.categorieInconnue && choix == ImportExportService::CategorieInconnue::Ignorer) {
            statut = tr("Ignorée (catégorie inconnue)");
            couleur = QColor(QStringLiteral("#B45309"));
        } else if (ligne.categorieInconnue) {
            statut = choix == ImportExportService::CategorieInconnue::Creer
                         ? tr("OK (catégorie créée)") : tr("OK (catégorie « Autre »)");
            couleur = QColor(QStringLiteral("#B45309"));
            ++aImporter;
        } else {
            statut = tr("OK");
            couleur = QColor(QStringLiteral("#047857"));
            ++aImporter;
        }

        auto *celluleStatut = new QTableWidgetItem(statut);
        celluleStatut->setForeground(couleur);
        m_apercu->setItem(i, 0, celluleStatut);
        m_apercu->setItem(i, 1, new QTableWidgetItem(QString::number(ligne.numero)));
        m_apercu->setItem(i, 2, new QTableWidgetItem(ligne.date.toString(QStringLiteral("dd/MM/yyyy"))));
        m_apercu->setItem(i, 3, new QTableWidgetItem(ligne.nom));
        m_apercu->setItem(i, 4, new QTableWidgetItem(Formatage::montant(ligne.montant)));
        m_apercu->setItem(i, 5, new QTableWidgetItem(ligne.type));
        m_apercu->setItem(i, 6, new QTableWidgetItem(ligne.compte));
        m_apercu->setItem(i, 7, new QTableWidgetItem(ligne.categorie));
    }

    m_boutonImporter->setText(tr("Importer %n transaction(s)", nullptr, aImporter));
    m_boutonImporter->setEnabled(aImporter > 0);
}

void ImportExportPage::importer()
{
    QString erreur;
    const int nombre = m_service.importer(m_lignes, choixCategorieInconnue(), &erreur);
    if (nombre < 0) {
        QMessageBox::critical(this, tr("Import annulé"),
                              tr("Rien n'a été importé.\n%1").arg(erreur));
        return;
    }

    DataNotifier::instance().notifier();   // les autres ecrans se mettent a jour
    QMessageBox::information(this, tr("Import terminé"),
                             tr("%n transaction(s) importée(s).", nullptr, nombre));

    m_lignes.clear();
    m_fichierChoisi->setText(tr("Aucun fichier choisi (CSV, JSON ou XML)."));
    afficherApercu();
}

ImportExportService::CategorieInconnue ImportExportPage::choixCategorieInconnue() const
{
    return static_cast<ImportExportService::CategorieInconnue>(m_choixCategorie->currentData().toInt());
}
