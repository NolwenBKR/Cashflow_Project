#include "ResetDialog.h"

#include "../../core/services/DataNotifier.h"
#include "../../database/DatabaseManager.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Réinitialiser les données ?"));
    setMinimumWidth(460);

    auto *avertissement = new QLabel(
        tr("<b>Toutes vos données vont être supprimées.</b><br>"
           "Transactions, comptes, budgets et récurrences seront effacés définitivement. "
           "Pensez à exporter une sauvegarde avant."),
        this);
    avertissement->setWordWrap(true);

    m_garderCategories = new QCheckBox(tr("Conserver les catégories et étiquettes"), this);
    m_garderCategories->setChecked(true);

    auto *consigne = new QLabel(tr("Pour confirmer, tapez RÉINITIALISER :"), this);
    m_confirmation = new QLineEdit(this);
    m_confirmation->setPlaceholderText(QStringLiteral("RÉINITIALISER"));
    consigne->setBuddy(m_confirmation);

    auto *boutons = new QDialogButtonBox(this);
    boutons->addButton(tr("Annuler"), QDialogButtonBox::RejectRole);
    m_boutonReinitialiser = boutons->addButton(tr("Réinitialiser"), QDialogButtonBox::AcceptRole);
    m_boutonReinitialiser->setStyleSheet(QStringLiteral(
        "QPushButton { background: #B42318; color: white; padding: 6px 14px; border-radius: 6px; }"
        "QPushButton:disabled { background: #E8B4AF; }"));
    connect(boutons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(boutons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Le bouton reste desactive tant que le mot n'est pas tape exactement
    connect(m_confirmation, &QLineEdit::textChanged, this, &ResetDialog::mettreAJourBouton);
    mettreAJourBouton();

    auto *disposition = new QVBoxLayout(this);
    disposition->addWidget(avertissement);
    disposition->addWidget(m_garderCategories);
    disposition->addWidget(consigne);
    disposition->addWidget(m_confirmation);
    disposition->addWidget(boutons);
}

bool ResetDialog::garderCategories() const
{
    return m_garderCategories->isChecked();
}

void ResetDialog::mettreAJourBouton()
{
    // On accepte aussi REINITIALISER sans accent, plus facile a taper au clavier
    const QString texte = m_confirmation->text();
    m_boutonReinitialiser->setEnabled(texte == QStringLiteral("RÉINITIALISER")
                                      || texte == QStringLiteral("REINITIALISER"));
}

bool ResetDialog::demanderEtReinitialiser(QWidget *parent)
{
    ResetDialog dialogue(parent);
    if (dialogue.exec() != QDialog::Accepted) {
        return false;
    }

    DatabaseManager &base = DatabaseManager::instance();
    if (!base.resetAllData(dialogue.garderCategories())) {
        QMessageBox::critical(parent, tr("Erreur"),
                              tr("La réinitialisation a échoué :\n%1").arg(base.lastError()));
        return false;
    }

    DataNotifier::instance().notifier();   // tous les ecrans se vident d'eux-memes
    QMessageBox::information(parent, tr("Réinitialisation"),
                             tr("Toutes les données ont été supprimées."));
    return true;
}
