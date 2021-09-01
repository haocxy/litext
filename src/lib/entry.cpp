#include <set>
#include <iostream>

#include <QApplication>
#include <QPainter>

#include "core/logger.h"
#include "gui/config.h"
#include "gui/engine.h"
#include "gui/qt/main_window.h"
#include "doc/dbfiles.h"

#include "cmdopt.h"


static void useDrawText()
{
    QPixmap pix(32, 32);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0");
}


int entry(int argc, char *argv[])
{
    //sk_sp<SkSurface> surface = SkSurface::MakeRaster(SkImageInfo::Make(800, 600, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kPremul_SkAlphaType));

    //if (surface) {
    //    std::cout << "skia surface created" << std::endl;
    //} else {
    //    std::cout << "cannot create skia surface" << std::endl;
    //}

    try {

        // 解析程序命令行参数
        CmdOpt cmdOpt(argc, argv);
        if (cmdOpt.needHelp()) {
            cmdOpt.help(std::cout);
            return 0;
        }

        // 初始化日志模块
        logger::control::Option logOpt;
        logOpt.setLevel(cmdOpt.logLevel());
        logOpt.setDir("./tmp/log");
        logOpt.setBasename("notesharplog");
        logOpt.setWriteToStdout(cmdOpt.shouldLogToStdout());
        logger::control::init(logOpt);

        gui::Engine engine;

        // 删除无用的由程序创建的辅助文件
        doc::dbfiles::removeUselessDbFiles();

        const std::vector<doc::OpenInfo> &files = cmdOpt.files();
        for (const doc::OpenInfo &e : files) {
            LOGI << "open info: file [" << e.file() << "], row [" << e.row() << "]";
        }

        QApplication app(argc, argv);

        // 在 Windows 平台发现窗口首次打开时会有一段时间全部为白色，
        // 调查后发现是卡在了 QPainter::drawText(...) 的首次有效调用，
        // 虽然没有看 Qt 的内部实现，但猜测是由于某种延迟加载的机制导致的，
        // 所以解决办法就是在窗口显示前提前使用这个函数（注意，绘制的文本不能为空字符串）
        useDrawText();

        gui::Config config;

        gui::qt::MainWindow mainWindow(engine, config);
        if (!cmdOpt.files().empty()) {
            const doc::OpenInfo &info = cmdOpt.files()[0];
            mainWindow.openDocument(info.file(), info.row());
        }
        mainWindow.show();

        return app.exec();
    }
    catch (const std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return 1;
    }
}
