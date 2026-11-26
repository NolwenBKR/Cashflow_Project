#ifndef ACCOUNTDATA_H
#define ACCOUNTDATA_H

#include <QDate>
#include <QString>
#include <QVector>

struct AccountOperation {
    QString label;
    qint64 amount = 0;      // positif = entrée, négatif = sortie
    QDate date;
};

struct AccountData {
    int id = 0;
    QString name;
    QString type;           // "Mobile money", "Épargne", "Espèces"
    QString color;          // "#RRGGBB" (vide = couleur par défaut)
    qint64 balance = 0;
    int monthTransactions = 0;
    qint64 monthIn = 0;     // total des entrées du mois (positif)
    qint64 monthOut = 0;    // total des sorties du mois (négatif)
    QVector<AccountOperation> recent;   // la plus récente d'abord
};

#endif // ACCOUNTDATA_H
