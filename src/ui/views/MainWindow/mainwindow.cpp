#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../../core/router/AppRouter.h"

#include <QDebug>

namespace {
constexpr int RouteNameRole = Qt::UserRole + 1;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->centralwidget->layout()->setSpacing(0);
    auto *sideLayout = qobject_cast<QVBoxLayout *>(ui->sidebar->layout());
    if (sideLayout) {
        sideLayout->removeWidget(ui->sidebarSeparator);
        sideLayout->insertWidget(sideLayout->indexOf(ui->btnSettings), ui->sidebarSeparator);
    }

    setRoutes(AppRouter::instance().routes());

    connect(ui->navList, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);
    ui->navList->setCurrentRow(0);

    connect(ui->btnSettings, &QPushButton::clicked, this, [] {
        AppRouter::instance().navigateTo(QStringLiteral("settings"));
    });

    connect(&AppRouter::instance(), &AppRouter::routeChanged,
            this, &MainWindow::displayPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setRoutes(const QList<Route> &routes) {
    ui->navList->clear();
    for (const Route &route : routes) {
        auto *item = new QListWidgetItem(route.icon(), route.label(), ui->navList);
        item->setData(RouteNameRole, route.name());
    }
}

void MainWindow::displayPage(const QString &routeName, QWidget *page) {
    if (!page) {
        return;
    }
    if (routeName != "settings") {
        ui->btnSettings->setChecked(false);
    }
    if (ui->stackedPages->indexOf(page) == -1) {
        ui->stackedPages->addWidget(page);
    }
    ui->stackedPages->setCurrentWidget(page);
    setActiveRoute(routeName);
}


void MainWindow::setActiveRoute(const QString &routeName) {
    for (int i = 0; i < ui->navList->count(); ++i) {
        QListWidgetItem *item = ui->navList->item(i);
        if (item && item->data(RouteNameRole).toString() == routeName) {
            ui->navList->setCurrentRow(i);
            return;
        }
    }
    ui->navList->clearSelection();
}

void MainWindow::onItemClicked(QListWidgetItem *item) {
    if (!item) {
        return;
    }
    const QString routeName = item->data(RouteNameRole).toString();
    if (!routeName.isEmpty()) {
        AppRouter::instance().navigateTo(routeName);
    }
}
