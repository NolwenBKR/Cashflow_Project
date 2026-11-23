#include "Sidebar.h"
#include <QVBoxLayout>
#include "../../core/router/AppRouter.h"

namespace {
constexpr int RouteNameRole = Qt::UserRole + 1;
}

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_listWidget = new QListWidget(this);
    m_listWidget->setMinimumWidth(180);
    m_listWidget->setMaximumWidth(260);

    layout->addWidget(m_listWidget);

    connect(m_listWidget, &QListWidget::itemClicked, this, &Sidebar::onItemClicked);
}

void Sidebar::setRoutes(const QList<Route> &routes) {
    m_listWidget->clear();
    for (const Route &route : routes) {
        auto *item = new QListWidgetItem(route.icon(), route.label(), m_listWidget);
        item->setData(RouteNameRole, route.name());
    }
}

void Sidebar::setActiveRoute(const QString &routeName) {
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem *item = m_listWidget->item(i);
        if (item && item->data(RouteNameRole).toString() == routeName) {
            m_listWidget->setCurrentRow(i);
            return;
        }
    }
}

void Sidebar::onItemClicked(QListWidgetItem *item) {
    if (!item) {
        return;
    }
    const QString routeName = item->data(RouteNameRole).toString();
    if (!routeName.isEmpty()) {
        AppRouter::instance().navigateTo(routeName);
    }
}
