#include "mainwindow.h"
#include <QApplication>

int main(int argc, char*argv[])

{
    QApplication app(argc, argv);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,true);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();

}
