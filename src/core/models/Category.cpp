#include "Category.h"

Category::Category(int id, const QString &nom, const QString &couleur)
    : m_id(id)
    , m_nom(nom)
    , m_couleur(couleur)
{
}

int Category::id() const
{
    return m_id;
}

void Category::setId(int id)
{
    m_id = id;
}

QString Category::nom() const
{
    return m_nom;
}

void Category::setNom(const QString &nom)
{
    m_nom = nom;
}

QString Category::couleur() const
{
    return m_couleur;
}

void Category::setCouleur(const QString &couleur)
{
    m_couleur = couleur;
}
