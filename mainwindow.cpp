#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->navList->clear();
    ui->navList->addItems({
        "Tableau de bord",
        "Transactions",
        "Comptes",
        "Catégories & étiquettes",
        "Budgets",
        "Récurrences",
        "Rapports",
        "Bourse",
        "Import / Export"
    });

    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->centralwidget->layout()->setSpacing(0);
    auto *sideLayout = qobject_cast<QVBoxLayout *>(ui->sidebar->layout());
    if (sideLayout) {
        sideLayout->removeWidget(ui->sidebarSeparator);
        sideLayout->insertWidget(sideLayout->indexOf(ui->btnSettings), ui->sidebarSeparator);
    }

    connect(ui->navList, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row < 0) return;
        ui->btnSettings->setChecked(false);
        ui->stackedPages->setCurrentIndex(row);
    });
    connect(ui->btnSettings, &QPushButton::clicked, this, [this] {
        ui->btnSettings->setChecked(true);
        ui->navList->clearSelection();
        ui->navList->setCurrentRow(-1);
        ui->stackedPages->setCurrentIndex(9);
    });
    ui->navList->setCurrentRow(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
