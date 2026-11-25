#include "Category.h"

Category::Category(int id, const QString &nom, const QString &couleur, const QString &type)
    : m_nom(nom)
    , m_couleur(couleur)
    , m_type(type)
{
    setId(id);
}

QString Category::nom() const { return m_nom; }
void Category::setNom(const QString &nom) { m_nom = nom; }

QString Category::couleur() const { return m_couleur; }
void Category::setCouleur(const QString &couleur) { m_couleur = couleur; }

QString Category::type() const { return m_type; }
void Category::setType(const QString &type) { m_type = type; }
