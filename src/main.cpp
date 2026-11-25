#include "ui/views/MainWindow/mainwindow.h"
#include "core/router/AppRouter.h"
#include "resources/routes/routes.h"

#include <QApplication>
#include <QFile>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppRouter::instance().init(buildAppRoutes());


    QFontDatabase::addApplicationFont(":/fonts/Inter_24pt-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Inter_24pt-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/SpaceGrotesk-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/SpaceGrotesk-Bold.ttf");

    QFile f(":/styles/light.qss");
    if (f.open(QFile::ReadOnly))
        a.setStyleSheet(QString::fromUtf8(f.readAll()));

    MainWindow w;
    w.resize(960, 640);
    w.show();

    AppRouter::instance().navigateTo(QStringLiteral("home"));

    return a.exec();
}
