#ifndef BUDGET_H
#define BUDGET_H

#include <QDate>
#include <QList>
#include <QString>
#include <QtGlobal>
#include "BaseModel.h"

class Budget : public BaseModel<Budget>
{
public:
    Budget() = default;
    // periode : "hebdomadaire", "mensuelle" ou "annuelle"
    Budget(int id,
           const QString &nom,
           qint64 plafond,
           const QString &periode,
           const QDate &dateDebut,
           const QDate &dateFin,
           int seuilAlerte = 80,
           const QList<int> &categoryIds = QList<int>());

    QString nom() const;
    void setNom(const QString &nom);

    qint64 plafond() const;
    void setPlafond(qint64 plafond);

    QString periode() const;
    void setPeriode(const QString &periode);

    QDate dateDebut() const;
    void setDateDebut(const QDate &dateDebut);

    QDate dateFin() const;
    void setDateFin(const QDate &dateFin);

    // Pourcentage a partir duquel on alerte l'utilisateur (80 par defaut)
    int seuilAlerte() const;
    void setSeuilAlerte(int seuilAlerte);

    // Un budget peut couvrir une ou plusieurs categories
    QList<int> categoryIds() const;
    void setCategoryIds(const QList<int> &categoryIds);

private:
    QString m_nom;
    qint64 m_plafond = 0;
    QString m_periode = QStringLiteral("mensuelle");
    QDate m_dateDebut;
    QDate m_dateFin;
    int m_seuilAlerte = 80;
    QList<int> m_categoryIds;
};

#endif // BUDGET_H
