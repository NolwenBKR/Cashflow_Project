#include "Compte.h"

Compte::Compte(int id,
               const QString &nom,
               int solde,
               int soldeInitial,
               const QString &typeDeCompte)
    : m_id(id)
    , m_nom(nom)
    , m_solde(solde)
    , m_soldeInitial(soldeInitial)
    , m_typeDeCompte(typeDeCompte)
{
    setId(id);
}

QString Compte::nom() const
{
    return m_nom;
}

void Compte::setNom(const QString &nom)
{
    m_nom = nom;
}

int Compte::solde() const
{
    return m_solde;
}

void Compte::setSolde(int solde)
{
    m_solde = solde;
}

int Compte::soldeInitial() const
{
    return m_soldeInitial;
}

void Compte::setSoldeInitial(int initialSolde)
{
    m_soldeInitial= initialSolde;
}

QString Compte::typeDeCompte() const
{
    return m_typeDeCompte;
}

void Compte::setTypeDeCompte(const QString &typeDeCompte)
{
    m_typeDeCompte = typeDeCompte;
}

void Compte::modifierSolde(int delta)
{
    m_solde += delta;
}
