#include "ui/views/MainWindow/mainwindow.h"
#include "database/DatabaseManager.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Cashflood");

    // Dossier de données de l'application (ex. ~/.local/share/Cashflood sous Linux)
    const QString dossier = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dossier);
    const QString chemin = dossier + "/cashflood.db";

    // Ouvre la base et crée les tables avant d'afficher la fenêtre
    if (!DatabaseManager::instance().initDatabase(chemin)) {
        QMessageBox::critical(nullptr, "Erreur",
                              "Impossible d'ouvrir la base de données :\n"
                                  + DatabaseManager::instance().lastError());
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
