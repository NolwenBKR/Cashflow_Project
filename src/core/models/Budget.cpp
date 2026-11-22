#include "Budget.h"

Budget::Budget(int id,
               int plafond,
               int avancement,
               int categoryId,
               const QDate &dateDebut,
               const QDate &dateFin)
    : m_plafond(plafond)
    , m_avancement(avancement)
    , m_categoryId(categoryId)
    , m_dateDebut(dateDebut)
    , m_dateFin(dateFin)
{
    setId(id);
}

int Budget::plafond() const
{
    return m_plafond;
}

void Budget::setPlafond(int plafond)
{
    m_plafond = plafond;
}

int Budget::avancement() const
{
    return m_avancement;
}

void Budget::setAvancement(int avancement)
{
    m_avancement = avancement;
}

int Budget::categoryId() const
{
    return m_categoryId;
}

void Budget::setCategoryId(int categoryId)
{
    m_categoryId = categoryId;
}

QDate Budget::dateDebut() const
{
    return m_dateDebut;
}

void Budget::setDateDebut(const QDate &dateDebut)
{
    m_dateDebut = dateDebut;
}

QDate Budget::dateFin() const
{
    return m_dateFin;
}

void Budget::setDateFin(const QDate &dateFin)
{
    m_dateFin = dateFin;
}
