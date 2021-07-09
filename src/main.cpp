#include <iostream>

#include <vector>
#include <string>

#include <QApplication>
#include <QDebug>

#include "gui/qt/run_on_qobject_event.h"
#include "gui/qt/mainwindow.h"

#include "core/time_util.h"
#include "doc/charset_detect_util.h"


static int charset(const std::vector<std::string> &args)
{
    std::string path;
    size_t offset = 0;
    size_t len = 0;
    if (args.empty()) {
        std::cerr << "bad argument count" << std::endl;
        return 1;
    }
    if (args.size() > 0) {
        path = args[0];
    }
    if (args.size() > 1) {
        offset = std::stoi(args[1]);
    }
    if (args.size() > 2) {
        len = std::stoi(args[2]);
    }
    ElapsedTime elapsedTime;
    std::string charset = doc::CharsetDetectUtil::detectCharsetOfFile(path, offset, len);
    ElapsedTime::MilliSec usage = elapsedTime.milliSec();
    std::cout << "path: " << path << std::endl;
    std::cout << "range: " << offset << "," << len << std::endl;
    std::cout << "charset: " << charset << std::endl;
    std::cout << "time usage: " << usage << "ms" << std::endl;
    std::cout << std::endl;
    return 0;
}

static int chooseDebug(const std::string &cmd, const std::vector<std::string> &args)
{
    if (cmd == "charset") {
        return charset(args);
    }

    std::cerr << "unknow cmd: [" << cmd << "]" << std::endl;
    return 1;
}

static int cmdTool(int argc, char *argv[])
{
    if (argc < 4) {
        std::cerr << "bad argument count" << std::endl;
        return 1;
    }

    if (std::string(argv[1]) != "cmd") {
        std::cerr << "first cmd should be 'd'" << std::endl;
        return 1;
    }

    std::vector<std::string> args;
    for (int i = 3; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
    }

    return chooseDebug(argv[2], args);
}

int main(int argc, char *argv[])
{
    if (argc > 2 && std::string(argv[1]) == "cmd") {
        return cmdTool(argc, argv);
    }

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
