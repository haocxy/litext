#include <iostream>

#include <vector>
#include <string>
#include <sstream>

#include <QApplication>
#include <QDebug>

#include "core/system_util.h"
#include "core/time_util.h"
#include "core/io_context_strand.h"
#include "core/logger.h"
#include "doc/document.h"
#include "gui/qt/mainwindow.h"
#include "gui/qt/gui_thread_worker.h"

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

static void test(const std::string &path)
{
    gui::qt::GuiThreadWorker guiThread;

    doc::Document doc(path, guiThread);
}

int main(int argc, char *argv[])
{
    LOGI << "Page Size: " << SystemUtil::pageSize();
    LOGI << "Processor Count: " << SystemUtil::processorCount();

    if (argc > 2 && std::string(argv[1]) == "cmd") {
        return cmdTool(argc, argv);
    }

    if (argc < 2) {
        LOGE << "Error: file path require, give it by commond line argument";
        return 1;
    }

    IOContextStrand::Pool pool;
    IOContextStrand strand1(pool, "StrandA");
    IOContextStrand strand2(pool, "StrandB");

    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    QApplication app(argc, argv);

    if (argc > 2 && args[1] == "test") {
        test(args[2]);
        return app.exec();
    } else {
        gui::qt::MainWindow mainWindow(argv[1]);
        mainWindow.show();
        return app.exec();
    }
}
