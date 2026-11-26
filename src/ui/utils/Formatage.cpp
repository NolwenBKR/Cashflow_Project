#include "Formatage.h"

#include <QColor>
#include <QLocale>
#include <QPainter>
#include <QPixmap>

namespace Formatage {

QString montant(qint64 valeur)
{
    return QLocale(QLocale::French).toString(valeur) + QStringLiteral(" FCFA");
}

QString montantSigne(qint64 valeur)
{
    const QString signe = valeur >= 0 ? QStringLiteral("+ ") : QStringLiteral("\u2212 ");
    return signe + montant(qAbs(valeur));
}

QIcon pastille(const QString &couleur, int taille)
{
    QPixmap image(taille, taille);
    image.fill(Qt::transparent);

    QPainter peintre(&image);
    peintre.setRenderHint(QPainter::Antialiasing);
    peintre.setPen(Qt::NoPen);
    peintre.setBrush(QColor(couleur.isEmpty() ? QStringLiteral("#5B6B6A") : couleur));
    peintre.drawEllipse(0, 0, taille - 1, taille - 1);

    return QIcon(image);
}

} // namespace Formatage
