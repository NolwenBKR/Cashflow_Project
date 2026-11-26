#ifndef COMPTESPAGE_H
#define COMPTESPAGE_H

#include "../../../database/repositories/AccountRepository.h"
#include "../../../database/repositories/TransactionRepository.h"

#include <QWidget>

class QLabel;
class QListWidget;
class QPushButton;

/**
 * @brief Page Comptes : liste des comptes, solde total et detail du compte choisi.
 *
 * Permet de creer, modifier, supprimer et reorganiser les comptes
 * (boutons monter/descendre ou glisser-deposer dans la liste).
 */
class ComptesPage : public QWidget
{
    Q_OBJECT

public:
    explicit ComptesPage(QWidget *parent = nullptr);

public slots:
    // Recharge tous les comptes depuis la base et met a jour l'affichage
    void rafraichir();

private:
    void creerCompte();
    void modifierCompte();
    void supprimerCompte();
    void deplacerSelection(int decalage);   // -1 = monter, +1 = descendre
    void enregistrerOrdreDeLaListe();
    void afficherDetail();
    void mettreAJourBoutons();
    int idSelectionne() const;
    QList<int> idsDansLOrdreAffiche() const;

    AccountRepository m_comptes;
    TransactionRepository m_transactions;

    bool m_chargementEnCours = false;   // evite de reagir a nos propres changements de liste
    int m_idAReselectionner = -1;

    QLabel *m_total = nullptr;
    QListWidget *m_liste = nullptr;
    QPushButton *m_boutonMonter = nullptr;
    QPushButton *m_boutonDescendre = nullptr;
    QPushButton *m_boutonModifier = nullptr;
    QPushButton *m_boutonSupprimer = nullptr;

    QLabel *m_detailTitre = nullptr;
    QLabel *m_detailSolde = nullptr;
    QLabel *m_detailEntrees = nullptr;
    QLabel *m_detailSorties = nullptr;
    QListWidget *m_detailOperations = nullptr;
};

#endif // COMPTESPAGE_H
