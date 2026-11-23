#include "Category.h"

Category::Category(int id, const QString &nom, const QString &couleur)
    : m_nom(nom)
    , m_couleur(couleur)
{
    setId(id);
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
