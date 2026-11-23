#include "AppRouter.h"

AppRouter& AppRouter::instance() {
    static AppRouter router;
    return router;
}

AppRouter::AppRouter(QObject *parent)
    : QObject(parent) {}

void AppRouter::init(const QList<Route> &routes) {
    m_routes = routes;
    m_currentRoute.clear();
    m_currentPage = nullptr;
}

Route* AppRouter::findRoute(const QString &routeName) {
    for (Route &route : m_routes) {
        if (route.name() == routeName) {
            return &route;
        }
    }
    return nullptr;
}

void AppRouter::navigateTo(const QString &routeName, const QVariantMap &params) {
    Route *route = findRoute(routeName);
    if (!route) {
        return;
    }

    QWidget *page = route->resolve(params);
    if (!page) {
        return;
    }

    m_currentRoute = routeName;
    m_currentPage = page;
    emit routeChanged(routeName, page);
}
