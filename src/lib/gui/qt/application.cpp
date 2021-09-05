#include "application.h"


#include <QPixmap>
#include <QPainter>

#include "core/system.h"
#include "core/logger.h"
#include "doc/dbfiles.h"

#include "main_window.h"


namespace gui::qt
{

static void useDrawTextForWindows()
{
#ifndef WIN32
    return;
#endif
    QPixmap pix(32, 32);
    QPainter painter(&pix);
    painter.drawText(0, 0, "0");
}

Application::Application(const InitInfo &initInfo)
{
    engine_.init(initInfo);

    // 初始化Qt框架并构造QApplication
    initQtApp();

    // 初始化主窗口
    initMainWindow();

    // 打开由程序启动参数指定的文件
    openFiles(initInfo.openInfos());
}

Application::~Application()
{
    delete mainWindow_;
    mainWindow_ = nullptr;
}

int Application::exec()
{
    try {
        return QApplication::exec();
    } catch (const std::exception &e) {
        LOGE << "Application::exec() uncatched std::exception in gui thread, what: " << e.what();
        return 1;
    } catch (...) {
        LOGE << "Application::exec() uncatched unkown exception in gui thread";
        return 1;
    }
}

void Application::initQtApp()
{
    static int gFakeArgc = 1;
    static char *gFakeArgv[] = { "litext" };
    qtApp_ = std::make_unique<QApplication>(gFakeArgc, gFakeArgv);

    // 解决windows平台Qt框架首次绘制文本时长时间卡顿的问题
    // 在首个窗口创建前调用,虽然依然会卡顿,但不会导致窗口卡顿
    // 在窗口启动前卡顿,用户只会觉得启动事件稍长,是可以接收的效果
    // 但如果在窗口启动过程中卡顿,则窗口会有明显的一段时间没有内容,体验很差
    useDrawTextForWindows();
}

void Application::initMainWindow()
{
    mainWindow_ = new MainWindow(engine_, engine_.config());
    mainWindow_->show();
}

void Application::openFiles(const std::vector<doc::OpenInfo> &openInfos)
{
    for (const auto &info : openInfos) {
        mainWindow_->openDocument(info.file(), info.row());
    }
}

}
