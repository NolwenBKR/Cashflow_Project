#include "ComptesPage.h"

#include "../../dialogs/CompteDialog.h"
#include "../../utils/Formatage.h"
#include "../../../core/services/DataNotifier.h"

#include <QDate>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <utility>

ComptesPage::ComptesPage(QWidget *parent)
    : QWidget(parent)
{
    // --- En-tete : titre + bouton "Nouveau compte"
    auto *titre = new QLabel(tr("Comptes"), this);
    titre->setStyleSheet(QStringLiteral("font-size: 24px; font-weight: 700;"));
    auto *boutonNouveau = new QPushButton(tr("+ Nouveau compte"), this);

    auto *entete = new QHBoxLayout;
    entete->addWidget(titre);
    entete->addStretch();
    entete->addWidget(boutonNouveau);

    // --- Solde total
    m_total = new QLabel(this);
    m_total->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: 600;"));

    // --- Colonne de gauche : la liste + ses boutons
    m_liste = new QListWidget(this);
    m_liste->setIconSize(QSize(14, 14));
    m_liste->setSpacing(4);
    m_liste->setDragDropMode(QAbstractItemView::InternalMove);   // glisser-deposer pour reordonner
    m_liste->setDefaultDropAction(Qt::MoveAction);

    m_boutonMonter = new QPushButton(tr("Monter"), this);
    m_boutonDescendre = new QPushButton(tr("Descendre"), this);
    m_boutonModifier = new QPushButton(tr("Modifier"), this);
    m_boutonSupprimer = new QPushButton(tr("Supprimer"), this);

    auto *boutonsListe = new QHBoxLayout;
    boutonsListe->addWidget(m_boutonMonter);
    boutonsListe->addWidget(m_boutonDescendre);
    boutonsListe->addStretch();
    boutonsListe->addWidget(m_boutonModifier);
    boutonsListe->addWidget(m_boutonSupprimer);

    auto *astuce = new QLabel(tr("Astuce : glissez un compte dans la liste pour changer l'ordre."), this);
    astuce->setStyleSheet(QStringLiteral("color: #5B6B6A;"));

    auto *colonneGauche = new QVBoxLayout;
    colonneGauche->addWidget(m_liste);
    colonneGauche->addLayout(boutonsListe);
    colonneGauche->addWidget(astuce);

    // --- Colonne de droite : le detail du compte selectionne
    auto *detail = new QGroupBox(tr("Détail"), this);
    m_detailTitre = new QLabel(detail);
    m_detailTitre->setStyleSheet(QStringLiteral("font-size: 16px; font-weight: 600;"));
    m_detailSolde = new QLabel(detail);
    m_detailSolde->setStyleSheet(QStringLiteral("font-size: 22px; font-weight: 700;"));
    m_detailEntrees = new QLabel(detail);
    m_detailSorties = new QLabel(detail);
    m_detailOperations = new QListWidget(detail);
    m_detailOperations->setSelectionMode(QAbstractItemView::NoSelection);

    auto *colonneDroite = new QVBoxLayout(detail);
    colonneDroite->addWidget(m_detailTitre);
    colonneDroite->addWidget(m_detailSolde);
    colonneDroite->addWidget(m_detailEntrees);
    colonneDroite->addWidget(m_detailSorties);
    colonneDroite->addWidget(new QLabel(tr("Dernières opérations"), detail));
    colonneDroite->addWidget(m_detailOperations);

    auto *colonnes = new QHBoxLayout;
    colonnes->addLayout(colonneGauche, 3);
    colonnes->addWidget(detail, 2);

    auto *page = new QVBoxLayout(this);
    page->addLayout(entete);
    page->addWidget(m_total);
    page->addLayout(colonnes);

    // --- Connexions
    connect(boutonNouveau, &QPushButton::clicked, this, &ComptesPage::creerCompte);
    connect(m_boutonModifier, &QPushButton::clicked, this, &ComptesPage::modifierCompte);
    connect(m_boutonSupprimer, &QPushButton::clicked, this, &ComptesPage::supprimerCompte);
    connect(m_boutonMonter, &QPushButton::clicked, this, [this] { deplacerSelection(-1); });
    connect(m_boutonDescendre, &QPushButton::clicked, this, [this] { deplacerSelection(+1); });
    connect(m_liste, &QListWidget::itemDoubleClicked, this, &ComptesPage::modifierCompte);
    connect(m_liste, &QListWidget::currentRowChanged, this, [this] {
        if (!m_chargementEnCours) {
            afficherDetail();
            mettreAJourBoutons();
        }
    });
    // Apres un glisser-deposer, la liste a change d'ordre : on l'enregistre
    connect(m_liste->model(), &QAbstractItemModel::rowsMoved, this, [this] {
        if (!m_chargementEnCours) {
            enregistrerOrdreDeLaListe();
        }
    });
    // Quand n'importe quel ecran modifie les donnees, on se recharge
    connect(&DataNotifier::instance(), &DataNotifier::donneesModifiees,
            this, &ComptesPage::rafraichir);

    rafraichir();
}

void ComptesPage::rafraichir()
{
    m_chargementEnCours = true;

    const int idAvant = m_idAReselectionner > 0 ? m_idAReselectionner : idSelectionne();
    m_idAReselectionner = -1;
    m_liste->clear();

    QString erreur;
    const QList<Compte> comptes = m_comptes.findAll(&erreur);
    if (!erreur.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), erreur);
    }

    qint64 total = 0;
    int ligneASelectionner = comptes.isEmpty() ? -1 : 0;
    for (const Compte &compte : comptes) {
        auto *item = new QListWidgetItem(
            Formatage::pastille(compte.couleur()),
            QStringLiteral("%1\n%2  ·  %3").arg(compte.nom(), compte.typeDeCompte(),
                                                Formatage::montant(compte.solde())),
            m_liste);
        item->setData(Qt::UserRole, compte.id());
        if (compte.id() == idAvant) {
            ligneASelectionner = m_liste->count() - 1;
        }
        total += compte.solde();
    }

    m_total->setText(tr("Solde total : %1  ·  %n compte(s)", nullptr, comptes.size())
                         .arg(Formatage::montant(total)));
    m_liste->setCurrentRow(ligneASelectionner);

    m_chargementEnCours = false;
    afficherDetail();
    mettreAJourBoutons();
}

void ComptesPage::creerCompte()
{
    CompteDialog dialogue(Compte(), this);
    if (dialogue.exec() != QDialog::Accepted) {
        return;
    }

    Compte compte = dialogue.compte();
    QString erreur;
    if (!m_comptes.create(compte, &erreur)) {
        QMessageBox::warning(this, tr("Erreur"), tr("Impossible de créer le compte :\n%1").arg(erreur));
        return;
    }

    m_idAReselectionner = compte.id();       // le nouveau compte sera selectionne
    DataNotifier::instance().notifier();     // tous les ecrans se mettent a jour
}

void ComptesPage::modifierCompte()
{
    const int id = idSelectionne();
    if (id <= 0) {
        return;
    }

    CompteDialog dialogue(m_comptes.findById(id), this);
    if (dialogue.exec() != QDialog::Accepted) {
        return;
    }

    QString erreur;
    if (!m_comptes.update(dialogue.compte(), &erreur)) {
        QMessageBox::warning(this, tr("Erreur"), tr("Impossible de modifier le compte :\n%1").arg(erreur));
        return;
    }

    DataNotifier::instance().notifier();
}

void ComptesPage::supprimerCompte()
{
    const int id = idSelectionne();
    if (id <= 0) {
        return;
    }

    // Attention : a cause du ON DELETE CASCADE, les transactions du compte partent avec lui
    const Compte compte = m_comptes.findById(id);
    const int nombreTransactions = m_transactions.findByAccountId(id).size();

    QString message = tr("Supprimer le compte « %1 » ?").arg(compte.nom());
    if (nombreTransactions > 0) {
        message += QStringLiteral("\n\n")
        + tr("Ses %n transaction(s) seront aussi supprimées.", nullptr, nombreTransactions);
    }
    message += QStringLiteral("\n") + tr("Cette action est irréversible.");

    const auto reponse = QMessageBox::question(this, tr("Supprimer le compte"), message,
                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reponse != QMessageBox::Yes) {
        return;
    }

    QString erreur;
    if (!m_comptes.remove(id, &erreur)) {
        QMessageBox::warning(this, tr("Erreur"), tr("Impossible de supprimer le compte :\n%1").arg(erreur));
        return;
    }

    DataNotifier::instance().notifier();
}

void ComptesPage::deplacerSelection(int decalage)
{
    const int ligne = m_liste->currentRow();
    const int nouvelleLigne = ligne + decalage;
    if (ligne < 0 || nouvelleLigne < 0 || nouvelleLigne >= m_liste->count()) {
        return;
    }

    QList<int> ids = idsDansLOrdreAffiche();
    std::swap(ids[ligne], ids[nouvelleLigne]);

    QString erreur;
    if (!m_comptes.updateOrder(ids, &erreur)) {
        QMessageBox::warning(this, tr("Erreur"), erreur);
        return;
    }

    m_idAReselectionner = ids.at(nouvelleLigne);   // le compte deplace reste selectionne
    DataNotifier::instance().notifier();
}

void ComptesPage::enregistrerOrdreDeLaListe()
{
    QString erreur;
    if (!m_comptes.updateOrder(idsDansLOrdreAffiche(), &erreur)) {
        QMessageBox::warning(this, tr("Erreur"), erreur);
    }
    m_idAReselectionner = idSelectionne();
    DataNotifier::instance().notifier();
}

void ComptesPage::afficherDetail()
{
    m_detailOperations->clear();

    const int id = idSelectionne();
    if (id <= 0) {
        m_detailTitre->setText(tr("Aucun compte"));
        m_detailSolde->setText(QString());
        m_detailEntrees->setText(tr("Cliquez sur « Nouveau compte » pour commencer."));
        m_detailSorties->setText(QString());
        return;
    }

    const Compte compte = m_comptes.findById(id);
    m_detailTitre->setText(QStringLiteral("%1 — %2").arg(compte.nom(), compte.typeDeCompte()));
    m_detailSolde->setText(Formatage::montant(compte.solde()));

    // Entrees et sorties du mois en cours
    const QDate aujourdhui = QDate::currentDate();
    const QList<Transaction> transactions = m_transactions.findByAccountId(id);   // les plus recentes d'abord
    qint64 entrees = 0;
    qint64 sorties = 0;
    for (const Transaction &t : transactions) {
        if (t.date().year() == aujourdhui.year() && t.date().month() == aujourdhui.month()) {
            if (t.type() == Transaction::Type::Approvisionnement) {
                entrees += t.montant();
            } else {
                sorties += t.montant();
            }
        }
    }
    m_detailEntrees->setText(tr("Entrées du mois : %1").arg(Formatage::montantSigne(entrees)));
    m_detailSorties->setText(tr("Sorties du mois : %1").arg(Formatage::montantSigne(-sorties)));

    // Les 5 dernieres operations
    for (int i = 0; i < transactions.size() && i < 5; ++i) {
        const Transaction &t = transactions.at(i);
        m_detailOperations->addItem(QStringLiteral("%1  ·  %2  ·  %3")
                                        .arg(t.date().toString(QStringLiteral("dd/MM")), t.nom(),
                                             Formatage::montantSigne(t.montantDelta())));
    }
    if (transactions.isEmpty()) {
        m_detailOperations->addItem(tr("Aucune opération pour l'instant."));
    }
}

void ComptesPage::mettreAJourBoutons()
{
    const int ligne = m_liste->currentRow();
    const bool selection = ligne >= 0;
    m_boutonModifier->setEnabled(selection);
    m_boutonSupprimer->setEnabled(selection);
    m_boutonMonter->setEnabled(selection && ligne > 0);
    m_boutonDescendre->setEnabled(selection && ligne < m_liste->count() - 1);
}

int ComptesPage::idSelectionne() const
{
    const QListWidgetItem *item = m_liste->currentItem();
    return item ? item->data(Qt::UserRole).toInt() : -1;
}

QList<int> ComptesPage::idsDansLOrdreAffiche() const
{
    QList<int> ids;
    for (int i = 0; i < m_liste->count(); ++i) {
        ids.append(m_liste->item(i)->data(Qt::UserRole).toInt());
    }
    return ids;
}
