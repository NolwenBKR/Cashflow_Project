#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>
#include "BaseModel.h"

class Category : public BaseModel<Category>
{
public:
    Category() = default;
    // type : "depense" ou "approvisionnement"
    Category(int id,
             const QString &nom,
             const QString &couleur = QString(),
             const QString &type = QStringLiteral("depense"));

    QString nom() const;
    void setNom(const QString &nom);

    QString couleur() const;
    void setCouleur(const QString &couleur);

    QString type() const;
    void setType(const QString &type);

private:
    QString m_nom;
    QString m_couleur;
    QString m_type = QStringLiteral("depense");
};

#endif // CATEGORY_H
