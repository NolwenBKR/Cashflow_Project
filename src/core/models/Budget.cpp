#include "Budget.h"

Budget::Budget(int id,
               const QString &nom,
               qint64 plafond,
               const QString &periode,
               const QDate &dateDebut,
               const QDate &dateFin,
               int seuilAlerte,
               const QList<int> &categoryIds)
    : m_nom(nom)
    , m_plafond(plafond)
    , m_periode(periode)
    , m_dateDebut(dateDebut)
    , m_dateFin(dateFin)
    , m_seuilAlerte(seuilAlerte)
    , m_categoryIds(categoryIds)
{
    setId(id);
}

QString Budget::nom() const { return m_nom; }
void Budget::setNom(const QString &nom) { m_nom = nom; }

qint64 Budget::plafond() const { return m_plafond; }
void Budget::setPlafond(qint64 plafond) { m_plafond = plafond; }

QString Budget::periode() const { return m_periode; }
void Budget::setPeriode(const QString &periode) { m_periode = periode; }

QDate Budget::dateDebut() const { return m_dateDebut; }
void Budget::setDateDebut(const QDate &dateDebut) { m_dateDebut = dateDebut; }

QDate Budget::dateFin() const { return m_dateFin; }
void Budget::setDateFin(const QDate &dateFin) { m_dateFin = dateFin; }

int Budget::seuilAlerte() const { return m_seuilAlerte; }
void Budget::setSeuilAlerte(int seuilAlerte) { m_seuilAlerte = seuilAlerte; }

QList<int> Budget::categoryIds() const { return m_categoryIds; }
void Budget::setCategoryIds(const QList<int> &categoryIds) { m_categoryIds = categoryIds; }
