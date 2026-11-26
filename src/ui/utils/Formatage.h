#ifndef FORMATAGE_H
#define FORMATAGE_H

#include <QIcon>
#include <QString>
#include <QtGlobal>

// Fonctions d'affichage communes a tous les ecrans
namespace Formatage {

// 1245300 -> "1 245 300 FCFA"
QString montant(qint64 valeur);

// 150000 -> "+ 150 000 FCFA", -18500 -> "− 18 500 FCFA"
QString montantSigne(qint64 valeur);

// Petite pastille ronde de la couleur donnee (ex. "#0F766E"), pour les listes
QIcon pastille(const QString &couleur, int taille = 14);

} // namespace Formatage

#endif // FORMATAGE_H
