#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "../../../core/router/Route.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setRoutes(const QList<Route> &routes);
    void setActiveRoute(const QString &routeName);

public slots:
    void displayPage(const QString &routeName, QWidget *page);
private slots:
    void onItemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
