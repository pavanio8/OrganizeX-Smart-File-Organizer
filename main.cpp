#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    QApplication::setApplicationName("OrganizeX");
    QApplication::setOrganizationName("OrganizeX");

    MainWindow window;

    window.show();

    return app.exec();
}