#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFontDatabase::addApplicationFont(":/fonts/Inter_24pt-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Inter_24pt-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/SpaceGrotesk-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/SpaceGrotesk-Bold.ttf");

    QFile f(":/light.qss");
    if (f.open(QFile::ReadOnly))
        a.setStyleSheet(QString::fromUtf8(f.readAll()));

    MainWindow w;
    w.show();
    return a.exec();
}
