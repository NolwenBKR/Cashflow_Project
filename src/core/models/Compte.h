#ifndef COMPTE_H
#define COMPTE_H

#include "BaseModel.h"

#include <QString>
#include <QtGlobal>

class Compte : public BaseModel<Compte>
{
public:
    Compte() = default;
    Compte(int id,
           const QString &nom,
           const QString &typeDeCompte,
           qint64 soldeInitial,
           const QString &couleur = QString(),
           int ordre = 0,
           qint64 solde = 0);

    QString nom() const;
    void setNom(const QString &nom);

    QString typeDeCompte() const;
    void setTypeDeCompte(const QString &typeDeCompte);

    qint64 soldeInitial() const;
    void setSoldeInitial(qint64 soldeInitial);

    QString couleur() const;
    void setCouleur(const QString &couleur);

    int ordre() const;
    void setOrdre(int ordre);

    // Solde actuel = solde initial + revenus - depenses.
    // Calcule par la base (AccountRepository), donc en lecture seule.
    qint64 solde() const;

private:
    QString m_nom;
    QString m_typeDeCompte;
    qint64 m_soldeInitial = 0;
    QString m_couleur;
    int m_ordre = 0;
    qint64 m_solde = 0;
};

#endif // COMPTE_H
