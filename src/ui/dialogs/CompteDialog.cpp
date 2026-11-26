#include "CompteDialog.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

CompteDialog::CompteDialog(const Compte &compte, QWidget *parent)
    : QDialog(parent)
    , m_compte(compte)
    , m_couleurs({"#D97706", "#0F766E", "#1D4ED8", "#7C3AED",
                  "#DB2777", "#047857", "#0891B2", "#DC2626"})
{
    const bool creation = compte.isNull();
    setWindowTitle(creation ? tr("Nouveau compte") : tr("Modifier le compte"));
    setMinimumWidth(420);

    // Nom
    m_nom = new QLineEdit(compte.nom(), this);
    m_nom->setPlaceholderText(tr("Ex. MTN MoMo"));

    // Type
    m_type = new QComboBox(this);
    m_type->addItems({tr("Mobile money"), tr("Compte épargne"), tr("Espèces"),
                      tr("Compte bancaire"), tr("Autre")});
    if (!creation) {
        if (m_type->findText(compte.typeDeCompte()) < 0) {
            m_type->addItem(compte.typeDeCompte());
        }
        m_type->setCurrentText(compte.typeDeCompte());
    }

    // Solde initial
    m_soldeInitial = new QSpinBox(this);
    m_soldeInitial->setRange(0, 999999999);
    m_soldeInitial->setSingleStep(1000);
    m_soldeInitial->setSuffix(tr(" FCFA"));
    m_soldeInitial->setGroupSeparatorShown(true);
    m_soldeInitial->setValue(static_cast<int>(compte.soldeInitial()));

    // Couleur : une pastille cliquable par couleur, une seule choisie a la fois
    auto *ligneCouleurs = new QHBoxLayout;
    m_groupeCouleurs = new QButtonGroup(this);
    m_groupeCouleurs->setExclusive(true);
    for (int i = 0; i < m_couleurs.size(); ++i) {
        auto *bouton = new QPushButton(this);
        bouton->setCheckable(true);
        bouton->setFixedSize(32, 32);
        bouton->setToolTip(m_couleurs.at(i));
        bouton->setAccessibleName(tr("Couleur %1").arg(m_couleurs.at(i)));
        bouton->setStyleSheet(QStringLiteral(
                                  "QPushButton { background: %1; border: 2px solid transparent; border-radius: 16px; }"
                                  "QPushButton:checked { border: 3px solid #13201F; }").arg(m_couleurs.at(i)));
        m_groupeCouleurs->addButton(bouton, i);
        ligneCouleurs->addWidget(bouton);
    }
    ligneCouleurs->addStretch();
    const int indexCouleur = m_couleurs.indexOf(compte.couleur());
    m_groupeCouleurs->button(indexCouleur >= 0 ? indexCouleur : 0)->setChecked(true);

    // Formulaire
    auto *formulaire = new QFormLayout;
    formulaire->addRow(tr("Nom du compte"), m_nom);
    formulaire->addRow(tr("Type"), m_type);
    formulaire->addRow(tr("Solde initial"), m_soldeInitial);
    formulaire->addRow(tr("Couleur"), ligneCouleurs);

    // Boutons Annuler / Enregistrer
    auto *boutons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save, this);
    m_boutonEnregistrer = boutons->button(QDialogButtonBox::Save);
    m_boutonEnregistrer->setText(creation ? tr("Créer le compte") : tr("Enregistrer"));
    boutons->button(QDialogButtonBox::Cancel)->setText(tr("Annuler"));
    connect(boutons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(boutons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // "Enregistrer" n'est actif que si le nom n'est pas vide
    connect(m_nom, &QLineEdit::textChanged, this, &CompteDialog::mettreAJourBoutonEnregistrer);
    mettreAJourBoutonEnregistrer();

    auto *disposition = new QVBoxLayout(this);
    disposition->addLayout(formulaire);
    disposition->addWidget(boutons);
}

Compte CompteDialog::compte() const
{
    Compte resultat = m_compte; // garde l'id et l'ordre
    resultat.setNom(m_nom->text().trimmed());
    resultat.setTypeDeCompte(m_type->currentText());
    resultat.setSoldeInitial(m_soldeInitial->value());
    const int index = m_groupeCouleurs->checkedId();
    resultat.setCouleur(index >= 0 ? m_couleurs.at(index) : QString());
    return resultat;
}

void CompteDialog::mettreAJourBoutonEnregistrer()
{
    m_boutonEnregistrer->setEnabled(!m_nom->text().trimmed().isEmpty());
}
