#include "routes.h"

#include <QIcon>
#include <QLabel>
#include <QList>
#include <QString>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QWidget>

namespace {

QWidget* createSimplePage(const QString &title, const QVariantMap &params) {
    auto *page = new QWidget();
    page->setObjectName(QStringLiteral("page_%1").arg(title));

    auto *layout = new QVBoxLayout(page);

    auto *titleLabel = new QLabel(title, page);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    layout->addWidget(titleLabel);

    if (!params.isEmpty()) {
        layout->addWidget(new QLabel(
            QStringLiteral("Params : %1").arg(params.value(QStringLiteral("id")).toString()),
            page
        ));
    }

    layout->addStretch();
    return page;
}

Route makeRoute(const QString &name, const QString &label, const QString &iconName) {
    return Route(
        name,
        label,
        QIcon::fromTheme(iconName),
        [label](const QVariantMap &params) -> QWidget* {
            return createSimplePage(label, params);
        }
    );
}

} // namespace

QList<Route> buildAppRoutes() {
    return {
        makeRoute(QStringLiteral("home"),          QStringLiteral("Accueil"),                 QStringLiteral("go-home")),
        makeRoute(QStringLiteral("dashboard"),     QStringLiteral("Tableau de bord"),         QStringLiteral("view-statistics")),
        makeRoute(QStringLiteral("transactions"),  QStringLiteral("Transactions"),            QStringLiteral("view-history")),
        makeRoute(QStringLiteral("accounts"),      QStringLiteral("Comptes"),                 QStringLiteral("wallet-open")),
        makeRoute(QStringLiteral("categories"),    QStringLiteral("Catégories & étiquettes"), QStringLiteral("tag")),
        makeRoute(QStringLiteral("budgets"),       QStringLiteral("Budgets"),                 QStringLiteral("view-calendar-timeline")),
        makeRoute(QStringLiteral("recurrences"),   QStringLiteral("Récurrences"),             QStringLiteral("view-refresh")),
        makeRoute(QStringLiteral("reports"),       QStringLiteral("Rapports"),                QStringLiteral("document-properties")),
        makeRoute(QStringLiteral("stock_market"),  QStringLiteral("Bourse"),                  QStringLiteral("office-chart-line")),
        makeRoute(QStringLiteral("import_export"), QStringLiteral("Import / Export"),         QStringLiteral("document-swap")),
    };
}
