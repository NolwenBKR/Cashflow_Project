#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>

class QCheckBox;
class QLineEdit;
class QPushButton;

/**
 * @brief Confirmation de la reinitialisation de toutes les donnees.
 *
 * Le bouton "Reinitialiser" ne s'active que si l'utilisateur tape REINITIALISER.
 * Le plus simple : appeler ResetDialog::demanderEtReinitialiser(this) depuis un bouton.
 */
class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);

    bool garderCategories() const;

    // Ouvre la confirmation, reinitialise si l'utilisateur confirme, previent les ecrans.
    // Retourne true si la reinitialisation a eu lieu.
    static bool demanderEtReinitialiser(QWidget *parent);

private:
    void mettreAJourBouton();

    QCheckBox *m_garderCategories = nullptr;
    QLineEdit *m_confirmation = nullptr;
    QPushButton *m_boutonReinitialiser = nullptr;
};

#endif // RESETDIALOG_H
