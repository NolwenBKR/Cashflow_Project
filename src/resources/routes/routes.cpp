#include "routes.h"
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

namespace {

QWidget* createSimplePage(const QString &title, const QVariantMap &params) {
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);

    layout->addWidget(new QLabel(title, page));
    if (!params.isEmpty()) {
        layout->addWidget(new QLabel(QStringLiteral("Params : %1").arg(params.value("id").toString()), page));
    }
    layout->addStretch();
    return page;
}

} // namespace

QList<Route> buildAppRoutes() {
    return {
        Route(
            QStringLiteral("home"),
            QStringLiteral("Accueil"),
            QIcon::fromTheme(QStringLiteral("go-home")),
            [](const QVariantMap &params) -> QWidget* {
                return createSimplePage(QStringLiteral("Accueil"), params);
            }
        ),
        Route(
            QStringLiteral("settings"),
            QStringLiteral("Paramètres"),
            QIcon::fromTheme(QStringLiteral("preferences-system")),
            [](const QVariantMap &params) -> QWidget* {
                return createSimplePage(QStringLiteral("Paramètres"), params);
            }
        )
    };
}
