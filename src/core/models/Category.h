#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>
#include "BaseModel.h"

class Category : public BaseModel<Category>
{
public:
    Category() = default;
    Category(int id, const QString &nom, const QString &couleur = QString());

    QString nom() const;
    void setNom(const QString &nom);

    QString couleur() const;
    void setCouleur(const QString &couleur);

private:
    QString m_nom;
    QString m_couleur;
};

#endif // CATEGORY_H
