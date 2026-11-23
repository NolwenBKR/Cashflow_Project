#include "MainLayout.h"
#include <QHBoxLayout>

MainLayout::MainLayout(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_sidebar = new Sidebar(this);
    m_stack = new QStackedWidget(this);

    layout->addWidget(m_sidebar);
    layout->addWidget(m_stack, 1);
}

void MainLayout::setRoutes(const QList<Route> &routes) {
    m_sidebar->setRoutes(routes);
}

void MainLayout::displayPage(const QString &routeName, QWidget *page) {
    if (!page) {
        return;
    }
    if (m_stack->indexOf(page) == -1) {
        m_stack->addWidget(page);
    }
    m_stack->setCurrentWidget(page);
    m_sidebar->setActiveRoute(routeName);
}
