#include <iostream>

#include <vector>
#include <string>
#include <sstream>

#include <QApplication>
#include <QPixmap>
#include <QPainter>

#include "core/system_util.h"
#include "core/time_util.h"
#include "core/single_thread_strand.h"
#include "core/single_thread_strand_pool.h"
#include "core/logger.h"
#include "doc/document.h"
#include "gui/qt/mainwindow.h"
#include "gui/qt/gui_strand.h"

#include "tool/big_file_maker.h"

class CmdLine {
public:
    CmdLine(const std::vector<std::string> &args) : args_(args) {}

    template <typename T>
    CmdLine &arg(T &obj) {
        if (current_ < args_.size()) {
            std::istringstream ss(args_[current_++]);
            ss >> obj;
        }
        return *this;
    }

private:
    std::vector<std::string> args_;
    int current_ = 0;
};

static int makeBigFile(const std::vector<std::string> &args)
{
    tool::BigFileMaker maker;
    return maker.exec(args);
}

static int chooseCommand(const std::string &cmd, const std::vector<std::string> &args)
{
    CmdLine cmdLine(args);
    if (cmd == "make-big-file") {
        return makeBigFile(args);
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

    return chooseCommand(argv[2], args);
}

static void useDrawText()
{
    QPixmap pix(1000, 1000);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0123456789");
}

int entry(int argc, char *argv[])
{
    LOGI << "Page Size: " << SystemUtil::pageSize();
    LOGI << "Processor Count: " << SystemUtil::processorCount();

    if (argc > 2 && std::string(argv[1]) == "cmd") {
        return cmdTool(argc, argv);
    }

    QApplication app(argc, argv);

    // 在 Windows 平台发现窗口首次打开时会有一段时间全部为白色，
    // 调查后发现是卡在了 QPainter::drawText(...) 的首次有效调用，
    // 虽然没有看 Qt 的内部实现，但猜测是由于某种延迟加载的机制导致的，
    // 所以解决办法就是在窗口显示前提前使用这个函数（注意，绘制的文本不能为空字符串）
    useDrawText();

    gui::qt::MainWindow mainWindow(argc >= 2 ? argv[1] : "D:\\tmp\\bigtest.txt");
    mainWindow.show();
    return app.exec();
}
