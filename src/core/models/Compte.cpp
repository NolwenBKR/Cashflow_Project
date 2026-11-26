#include "Compte.h"

Compte::Compte(int id,
               const QString &nom,
               const QString &typeDeCompte,
               qint64 soldeInitial,
               const QString &couleur,
               int ordre,
               qint64 solde)
    : m_nom(nom)
    , m_typeDeCompte(typeDeCompte)
    , m_soldeInitial(soldeInitial)
    , m_couleur(couleur)
    , m_ordre(ordre)
    , m_solde(solde)
{
    setId(id);
}

QString Compte::nom() const { return m_nom; }
void Compte::setNom(const QString &nom) { m_nom = nom; }

QString Compte::typeDeCompte() const { return m_typeDeCompte; }
void Compte::setTypeDeCompte(const QString &typeDeCompte) { m_typeDeCompte = typeDeCompte; }

qint64 Compte::soldeInitial() const { return m_soldeInitial; }
void Compte::setSoldeInitial(qint64 soldeInitial) { m_soldeInitial = soldeInitial; }

QString Compte::couleur() const { return m_couleur; }
void Compte::setCouleur(const QString &couleur) { m_couleur = couleur; }

int Compte::ordre() const { return m_ordre; }
void Compte::setOrdre(int ordre) { m_ordre = ordre; }

qint64 Compte::solde() const { return m_solde; }
