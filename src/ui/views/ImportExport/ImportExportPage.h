#ifndef IMPORTEXPORTPAGE_H
#define IMPORTEXPORTPAGE_H

#include "../../../core/services/ImportExportService.h"

#include <QList>
#include <QWidget>

class QComboBox;
class QDateEdit;
class QLabel;
class QPushButton;
class QTableWidget;

/**
 * @brief Page Import / Export : exporter les transactions et importer un fichier
 *        CSV, JSON ou XML avec un apercu verifie avant d'ecrire en base.
 */
class ImportExportPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImportExportPage(QWidget *parent = nullptr);

private:
    void exporter();
    void choisirFichierAImporter();
    void afficherApercu();
    void importer();
    ImportExportService::CategorieInconnue choixCategorieInconnue() const;

    ImportExportService m_service;
    QList<LigneImport> m_lignes;   // lignes lues, en attente d'import

    // Export
    QComboBox *m_formatExport = nullptr;
    QDateEdit *m_debut = nullptr;
    QDateEdit *m_fin = nullptr;

    // Import
    QLabel *m_fichierChoisi = nullptr;
    QTableWidget *m_apercu = nullptr;
    QComboBox *m_choixCategorie = nullptr;
    QPushButton *m_boutonImporter = nullptr;
};

#endif // IMPORTEXPORTPAGE_H
