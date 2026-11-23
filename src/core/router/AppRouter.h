#ifndef APPROUTER_H
#define APPROUTER_H

#include <QList>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QWidget>
#include "Route.h"

class AppRouter : public QObject {
    Q_OBJECT
public:
    static AppRouter& instance();

    void init(const QList<Route> &routes);
    void navigateTo(const QString &routeName, const QVariantMap &params = {});

    QWidget* currentPage() const { return m_currentPage; }
    QString currentRoute() const { return m_currentRoute; }
    const QList<Route>& routes() const { return m_routes; }

signals:
    void routeChanged(const QString &routeName, QWidget *page);

private:
    explicit AppRouter(QObject *parent = nullptr);
    Route* findRoute(const QString &routeName);

    QList<Route> m_routes;
    QString m_currentRoute;
    QWidget *m_currentPage = nullptr;
};

#endif // APPROUTER_H
