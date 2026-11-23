#ifndef MAINLAYOUT_H
#define MAINLAYOUT_H

#include <QList>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include "../../core/router/Route.h"
#include "Sidebar.h"

class MainLayout : public QWidget {
    Q_OBJECT
public:
    explicit MainLayout(QWidget *parent = nullptr);

    void setRoutes(const QList<Route> &routes);
    void displayPage(const QString &routeName, QWidget *page);

private:
    Sidebar *m_sidebar = nullptr;
    QStackedWidget *m_stack = nullptr;
};

#endif // MAINLAYOUT_H
