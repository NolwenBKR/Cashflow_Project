#ifndef BUDGET_H
#define BUDGET_H

#include <QDate>

class Budget
{
public:
    Budget() = default;
    Budget(int id,
           int plafond,
           int avancement,
           int categoryId,
           const QDate &dateDebut,
           const QDate &dateFin);

    int id() const;
    void setId(int id);

    int plafond() const;
    void setPlafond(int plafond);

    int avancement() const;
    void setAvancement(int avancement);

    int categoryId() const;
    void setCategoryId(int categoryId);

    QDate dateDebut() const;
    void setDateDebut(const QDate &dateDebut);

    QDate dateFin() const;
    void setDateFin(const QDate &dateFin);

private:
    int m_id = 0;
    int m_plafond = 0;
    int m_avancement = 0;
    int m_categoryId = 0;
    QDate m_dateDebut;
    QDate m_dateFin;
};

#endif // BUDGET_H
