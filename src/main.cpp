#include "ui/views/MainWindow/mainwindow.h"
#include "core/router/AppRouter.h"
#include "resources/routes/routes.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppRouter::instance().init(buildAppRoutes());

    MainWindow w;
    w.resize(960, 640);
    w.show();

    AppRouter::instance().navigateTo(QStringLiteral("home"));

    return a.exec();
}
