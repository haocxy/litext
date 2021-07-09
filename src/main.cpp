#include <iostream>

#include <QApplication>
#include <QDebug>

#include "gui/qt/run_on_qobject_event.h"
#include "gui/qt/mainwindow.h"

#include "doc/charset_detect_util.h"


static int detect(int argc, char *argv[])
{
    std::string path;
    size_t offset = 0;
    size_t len = 0;
    if (argc == 1) {
        std::cerr << "bad argument count" << std::endl;
        return 1;
    }
    if (argc > 1) {
        path = argv[1];
    }
    if (argc > 2) {
        offset = std::atoi(argv[2]);
    }
    if (argc > 3) {
        len = std::atoi(argv[3]);
    }
    std::string charset = doc::CharsetDetectUtil::detectCharsetOfFile(path, offset, len);
    std::cout << "charset: " << charset << std::endl;
    return 0;
}


int main(int argc, char *argv[])
{
    return detect(argc, argv);

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
