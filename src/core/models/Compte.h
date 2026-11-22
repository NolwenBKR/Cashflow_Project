#ifndef COMPTE_H
#define COMPTE_H

#include <QString>

class Compte
{
public:
    Compte() = default;
    Compte(int id,
           const QString &nom,
           int solde,
           int initialSolde,
           const QString &typeDeCompte);

    // Pourquoi on utilise des getteurs et setteurs : par precauton, si on decide
    // de changer comment recuperer une valeur ou la mettre a jou, ce sera plus simple.

    int id() const;
    void setId(int id);

    QString nom() const;
    void setNom(const QString &nom);

    int solde() const;
    void setSolde(int solde);

    int soldeInitial() const;
    void setSoldeInitial(int initialSolde);

    QString typeDeCompte() const;
    void setTypeDeCompte(const QString &typeDeCompte);

    // ajuste le solde courant d'un delta (positif ou negatif)
    void modifierSolde(int delta);

private:
    // les m_ au debut des noms des attributs signifie member et ser a ne pas
    // les confondre avec les getteurs
    int m_id = 0;
    QString m_nom;
    int m_solde = 0;
    int m_soldeInitial = 0;
    QString m_typeDeCompte;
};

#endif // COMPTE_H
