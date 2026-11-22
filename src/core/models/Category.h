#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>

class Category
{
public:
    Category() = default;
    Category(int id, const QString &nom, const QString &couleur = QString());

    int id() const;
    void setId(int id);

    QString nom() const;
    void setNom(const QString &nom);

    QString couleur() const;
    void setCouleur(const QString &couleur);

private:
    int m_id = 0;
    QString m_nom;
    QString m_couleur;
};

#endif // CATEGORY_H
