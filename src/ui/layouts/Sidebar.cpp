#include "Sidebar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "../../core/router/AppRouter.h"

namespace {
constexpr int RouteNameRole = Qt::UserRole + 1;
}

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent) {
    setMinimumWidth(100);
    setMaximumWidth(260);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 12, 8, 12);
    mainLayout->setSpacing(8);

    setupHeader();
    setupNavigationList();
    setupFooter();

    mainLayout->addSpacing(8);
    mainLayout->addWidget(m_listWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(m_settingsButton);
    mainLayout->addWidget(m_themeButton);

    setBrandInfo(QStringLiteral("Ca$hfl00d"));
    setThemeLabel(QStringLiteral("Thème : Clair"));

    connect(m_listWidget, &QListWidget::itemClicked, this, &Sidebar::onItemClicked);
    connect(m_settingsButton, &QPushButton::clicked, this, [this]() {
        emit settingsClicked();
        AppRouter::instance().navigateTo(QStringLiteral("settings"));
    });
    connect(m_themeButton, &QPushButton::clicked, this, &Sidebar::themeToggleRequested);
}

void Sidebar::setupHeader() {
    auto *headerWidget = new QWidget(this);
    auto *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(8, 4, 8, 4);
    headerLayout->setSpacing(10);

    m_brandBadge = new QLabel(headerWidget);
    m_brandBadge->setAlignment(Qt::AlignCenter);
    m_brandBadge->setFixedSize(28, 28);

    m_brandTitle = new QLabel(headerWidget);
    m_brandTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    headerLayout->addWidget(m_brandBadge);
    headerLayout->addWidget(m_brandTitle);

    layout()->addWidget(headerWidget);
}

void Sidebar::setupNavigationList() {
    m_listWidget = new QListWidget(this);
//    m_listWidget->setFrameShape(QFrame::NoFrame);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void Sidebar::setupFooter() {
    m_settingsButton = new QPushButton(tr("Paramètres"), this);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    m_settingsButton->setFlat(true);

    m_themeButton = new QPushButton(this);
    m_themeButton->setCursor(Qt::PointingHandCursor);
    m_themeButton->setFlat(true);
}

void Sidebar::setBrandInfo(const QString &appName, const QString &badgeText) {
    if (m_brandBadge) {
        m_brandBadge->setText(badgeText);
    }
    if (m_brandTitle) {
        m_brandTitle->setText(appName);
    }
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
    m_listWidget->clearSelection();
}

void Sidebar::setThemeLabel(const QString &themeText) {
    if (m_themeButton) {
        m_themeButton->setText(themeText);
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
