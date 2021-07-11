#include <iostream>

#include <vector>
#include <string>
#include <sstream>

#include <QApplication>
#include <QDebug>

#include "core/system_util.h"
#include "core/time_util.h"
#include "doc/charset_detect_util.h"
#include "gui/qt/mainwindow.h"

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


static int charset(CmdLine &cmd)
{
    std::string path;
    size_t offset = 0;
    size_t len = 0;
    cmd.arg(path).arg(offset).arg(len);

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

static int quickCharset(CmdLine &cmd)
{
    std::string path;
    cmd.arg(path);

    ElapsedTime elapsedTime;
    std::string charset = doc::CharsetDetectUtil::quickDetectCharset(path);
    ElapsedTime::MilliSec usage = elapsedTime.milliSec();
    std::cout << "path: " << path << std::endl;
    std::cout << "file size: " << fs::file_size(path) << std::endl;
    std::cout << "charset: " << charset << std::endl;
    std::cout << "time usage: " << usage << "ms" << std::endl;
    std::cout << std::endl;
    return 0;
}

static int makeBigFile(const std::vector<std::string> &args)
{
    tool::BigFileMaker maker;
    return maker.exec(args);
}

static int chooseDebug(const std::string &cmd, const std::vector<std::string> &args)
{
    CmdLine cmdLine(args);
    if (cmd == "charset") {
        return charset(cmdLine);
    }
    if (cmd == "quick-charset") {
        return quickCharset(cmdLine);
    }
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

    return chooseDebug(argv[2], args);
}

int main(int argc, char *argv[])
{
    std::cout << "Page Size: " << SystemUtil::pageSize() << std::endl;
    std::cout << "Processor Count: " << SystemUtil::processorCount() << std::endl;

    if (argc > 2 && std::string(argv[1]) == "cmd") {
        return cmdTool(argc, argv);
    }

    if (argc < 2) {
        std::cerr << "Error: file path require, give it by commond line argument" << std::endl;
        return 1;
    }

    QApplication app(argc, argv);

    gui::qt::MainWindow mainWindow(argv[1]);
    mainWindow.show();

    return app.exec();
}
