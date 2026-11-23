#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <QList>
#include <QListWidget>
#include <QString>
#include <QWidget>
#include "../../core/router/Route.h"

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = nullptr);

    void setRoutes(const QList<Route> &routes);
    void setActiveRoute(const QString &routeName);

private:
    void onItemClicked(QListWidgetItem *item);

    QListWidget *m_listWidget = nullptr;
};

#endif // SIDEBAR_H
