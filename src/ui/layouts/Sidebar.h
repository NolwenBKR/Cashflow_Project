#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <QList>
#include <QListWidget>
#include <QString>
#include <QWidget>
#include "../../core/router/Route.h"

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class Route;

class Sidebar : public QWidget {
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

    void setBrandInfo(const QString &appName, const QString &badgeText = QStringLiteral(""));
    void setRoutes(const QList<Route> &routes);
    void setActiveRoute(const QString &routeName);
    void setThemeLabel(const QString &themeText);

signals:
    void settingsClicked();
    void themeToggleRequested();

private slots:
    void onItemClicked(QListWidgetItem *item);

private:
    void setupHeader();
    void setupNavigationList();
    void setupFooter();

    QLabel *m_brandBadge = nullptr;
    QLabel *m_brandTitle = nullptr;

    QListWidget *m_listWidget = nullptr;

    QPushButton *m_settingsButton = nullptr;
    QPushButton *m_themeButton = nullptr;
};
#endif // SIDEBAR_H
