#include "application.h"


#include <QPixmap>
#include <QPainter>

#include "core/system.h"
#include "core/logger.h"
#include "doc/dbfiles.h"

#include "main_window.h"


namespace
{

static int gFakeArgc = 1;
static char gFakeAppName[256]{};
static char *gFakeArgv[1]{};

class FakeArgvInit {
public:
    FakeArgvInit() {
        const char *fakeName = "litext";
        std::memcpy(gFakeAppName, fakeName, std::strlen(fakeName));
        gFakeArgv[0] = gFakeAppName;
    }
};

static const FakeArgvInit gFakeArgvInit;

}


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

Application::Application()
{}

void Application::init(const InitInfo &initInfo)
{
    engine_.init(initInfo);

    // 初始化Qt框架并构造QApplication
    initQtApp(initInfo);

    // 初始化系统托盘区
    initSystemTray(initInfo);

    // 初始化主窗口
    initMainWindow();

    // 绑定事件处理
    bindSignals(initInfo);

    // 显示用户界面
    activateUI();

    // 打开由程序启动参数指定的文件
    openFiles(initInfo.openInfos());
}

Application::~Application()
{
    delete trayIcon_;
    trayIcon_ = nullptr;

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



void Application::initQtApp(const InitInfo &initInfo)
{
    qtApp_ = std::make_unique<QApplication>(gFakeArgc, gFakeArgv);

    if (initInfo.shouldStartAsServer()) {
        QGuiApplication::setQuitOnLastWindowClosed(false);
    }

    // 解决windows平台Qt框架首次绘制文本时长时间卡顿的问题
    // 在首个窗口创建前调用,虽然依然会卡顿,但不会导致窗口卡顿
    // 在窗口启动前卡顿,用户只会觉得启动事件稍长,是可以接收的效果
    // 但如果在窗口启动过程中卡顿,则窗口会有明显的一段时间没有内容,体验很差
    useDrawTextForWindows();
}

void Application::initSystemTray(const InitInfo &initInfo)
{
    if (initInfo.shouldStartAsServer()) {
        trayMenu_ = std::make_unique<TrayMenu>();
        trayIcon_ = new QSystemTrayIcon(QIcon(":/tray_icon.ico"));
        trayIcon_->setContextMenu(trayMenu_.get());

        connect(trayMenu_.get(), &TrayMenu::qtSigActivateUI, this, &Application::activateUI);

        connect(trayMenu_.get(), &TrayMenu::qtSigQuit, this, [this] {
            trayIcon_->hide();
            QApplication::quit();
        });

        connect(trayIcon_, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick) {
                activateUI();
            }
        });

        trayIcon_->show();
    }
}

void Application::initMainWindow()
{
    mainWindow_ = new MainWindow(engine_, engine_.config());
}

void Application::openFile(const doc::OpenInfo &openInfo)
{
    mainWindow_->openDocument(openInfo.file(), openInfo.row());
}

void Application::openFiles(const std::vector<doc::OpenInfo> &openInfos)
{
    for (const auto &info : openInfos) {
        mainWindow_->openDocument(info.file(), info.row());
    }
}

void Application::bindSignals(const InitInfo &initInfo)
{
    if (initInfo.shouldStartAsServer()) {
        bindSignalsForSingletonServer();
    }
}

void Application::bindSignalsForSingletonServer()
{
    SingletonServer &server = engine_.singletonServer();

    connect(this, &Application::qtSigActivateUI, this, &Application::activateUI);
    sigConns_ += server.sigActivateUI().connect([this] {
        emit qtSigActivateUI();
    });

    connect(this, &Application::qtSigOpenFile, this, &Application::qtSlotOpenFile);
    using OpenInfos = std::vector<doc::OpenInfo>;
    sigConns_ += server.sigRecvOpenInfos().connect([this](const OpenInfos &openInfos) {
        for (const doc::OpenInfo &openInfo : openInfos) {
            emit qtSigOpenFile(QString::fromStdU32String(openInfo.file().generic_u32string()), openInfo.row());
        }
    });
}

void Application::qtSlotOpenFile(const QString &path, long long row)
{
    activateUI();
    openFile({ fs::path(path.toStdU32String()), row });
}

void Application::activateUI()
{
    mainWindow_->showNormal();
    mainWindow_->activateWindow();
}

}
