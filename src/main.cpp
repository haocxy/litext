#include <iostream>

#include <QApplication>
#include <QDebug>

#include "qt/mainwindow.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Error: file path require, give it by commond line argument" << std::endl;
        return 1;
    }

    QApplication app(argc, argv);

    MainWindow mainWindow(argv[1]);
    mainWindow.show();

    return app.exec();
}
