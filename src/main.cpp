#include <iostream>

#include <QApplication>
#include <QDebug>

#include "gui/qt/run_on_qobject_event.h"
#include "gui/qt/mainwindow.h"


int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Error: file path require, give it by commond line argument" << std::endl;
        return 1;
    }

    QApplication app(argc, argv);

    // 为自定义的消息申请消息类型枚举值
    gui::qt::RunOnQObjectEvent::g_type = static_cast<QEvent::Type>(QEvent::registerEventType());

    gui::qt::MainWindow mainWindow(argv[1]);
    mainWindow.show();

    return app.exec();
}
