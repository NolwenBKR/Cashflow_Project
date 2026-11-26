#ifndef COMPTEDIALOG_H
#define COMPTEDIALOG_H

#include "../../core/models/Compte.h"

#include <QDialog>
#include <QStringList>

class QButtonGroup;
class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

/**
 * @brief Boite de dialogue pour creer ou modifier un compte.
 *
 * Compte vide (Compte()) = creation ; compte existant = modification.
 * Apres exec() == QDialog::Accepted, recuperer la saisie avec compte().
 */
class CompteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompteDialog(const Compte &compte = Compte(), QWidget *parent = nullptr);

    // Le compte rempli avec la saisie (garde l'id et l'ordre du compte d'origine)
    Compte compte() const;

private:
    void mettreAJourBoutonEnregistrer();

    Compte m_compte;
    QLineEdit *m_nom = nullptr;
    QComboBox *m_type = nullptr;
    QSpinBox *m_soldeInitial = nullptr;
    QButtonGroup *m_groupeCouleurs = nullptr;
    QStringList m_couleurs;
    QPushButton *m_boutonEnregistrer = nullptr;
};

#endif // COMPTEDIALOG_H
