#ifndef DATANOTIFIER_H
#define DATANOTIFIER_H

#include <QObject>

/**
 * @brief Previent tous les ecrans quand les donnees changent.
 *
 * Apres chaque modification reussie (creer, modifier, supprimer, reinitialiser,
 * importer), on appelle DataNotifier::instance().notifier().
 * Chaque page qui affiche des donnees se connecte au signal donneesModifiees()
 * pour se recharger toute seule.
 */
class DataNotifier : public QObject
{
    Q_OBJECT

public:
    static DataNotifier &instance();

    // A appeler apres chaque modification reussie des donnees
    void notifier();

signals:
    void donneesModifiees();

private:
    DataNotifier() = default;
};

#endif // DATANOTIFIER_H
