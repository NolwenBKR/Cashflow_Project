#pragma once
#include <QString>
#include <QChar>

inline QString formatFcfa(qint64 value, bool showSign = false)
{
    QString digits = QString::number(qAbs(value));
    for (int i = digits.size() - 3; i > 0; i -= 3)
        digits.insert(i, QChar(0x00A0));
    QString sign;
    if (value < 0)                   sign = QString(QChar(0x2212)) + QChar(0x00A0);
    else if (showSign && value > 0)  sign = QStringLiteral("+") + QChar(0x00A0);
    return sign + digits + QChar(0x00A0) + QStringLiteral("FCFA");
}
