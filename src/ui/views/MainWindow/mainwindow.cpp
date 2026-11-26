#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "../Comptes/ComptesPage.h"
#include "../../dialogs/ResetDialog.h"
#include "../ImportExport/ImportExportPage.h"

#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("Ca$hfl00d"));
    resize(1100, 700);

    // ---------------------------------------------------------------
    // PROVISOIRE : onglets de test en attendant le squelette d'interface
    // (barre laterale) du membre 4, qui remplacera ce bloc.
    // ---------------------------------------------------------------
    auto *onglets = new QTabWidget(this);
    onglets->addTab(new ComptesPage(onglets), tr("Comptes"));
    onglets->addTab(new ImportExportPage(onglets), tr("Import / Export"));

    auto *parametres = new QWidget(onglets);
    auto *boutonReinitialiser = new QPushButton(tr("Réinitialiser les données…"), parametres);
    connect(boutonReinitialiser, &QPushButton::clicked, this, [this] {
        ResetDialog::demanderEtReinitialiser(this);
    });
    auto *dispositionParametres = new QVBoxLayout(parametres);
    dispositionParametres->addWidget(new QLabel(tr("Zone de danger"), parametres));
    dispositionParametres->addWidget(boutonReinitialiser);
    dispositionParametres->addStretch();
    onglets->addTab(parametres, tr("Paramètres"));

    setCentralWidget(onglets);
}

MainWindow::~MainWindow()
{
    delete ui;
}
