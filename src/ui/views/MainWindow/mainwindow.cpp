#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../../core/router/AppRouter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //    , ui(new Ui::MainWindow)
{
    //    ui->setupUi(this);
    m_mainLayout = new MainLayout(this);
    setCentralWidget(m_mainLayout);

    m_mainLayout->setRoutes(AppRouter::instance().routes());

    connect(&AppRouter::instance(), &AppRouter::routeChanged,
            m_mainLayout, &MainLayout::displayPage);
}

MainWindow::~MainWindow()
{
    //    delete ui;
}

