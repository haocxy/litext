#include "application.h"

#include <fstream>

#include <QLocalSocket>

#include "core/system.h"
#include "core/logger.h"


namespace gui::qt
{

static fs::path pathOfSingleProcLockFile()
{
    return SystemUtil::userHome() / ".notesharp" / "single.lock";
}

Application::Application(int argc, char *argv[])
    : QApplication(argc, argv)
{

}

Application::~Application()
{
    clientSock_ = nullptr;
    serverSock_ = nullptr;
}

int Application::exec(SingletonMode singletonMode)
{
    if (singletonMode == SingletonMode::NoSingleton) {
        return execAsNonSingleton();
    }

    // 尝试对单例逻辑加锁
    const bool lockSingletonLogicSucceed = tryLockSingletonLogic();

    // 如果单例模式加锁失败, 则说明服务端已经启动
    const bool serverAlreadyStarted = !lockSingletonLogicSucceed;

    const bool mustSingleton = singletonMode == SingletonMode::MustSingleton;

    if (serverAlreadyStarted) {
        return execAsClient(mustSingleton);
    } else {
        return execAsServer(mustSingleton);
    }
}

int Application::execAsNonSingleton()
{
    return QApplication::exec();
}

int Application::execAsServer(bool mustSingleton)
{
    serverSock_ = new QLocalServer(this);
    serverSock_->listen("notesharp_singleton");
    if (serverSock_->isListening()) {
        connect(serverSock_, &QLocalServer::newConnection, this, &Application::onNewConnection);
        LOGI << "local server started: [" << serverSock_->fullServerName().toStdU32String() << "]";
        return QApplication::exec();
    }

    LOGE << "cannot start local server for singleton mode";

    if (!mustSingleton) {
        return QApplication::exec();
    }

    return 1;
}

int Application::execAsClient(bool mustSingleton)
{
    throw std::logic_error("unimplemented");
}

static boost::interprocess::file_lock mkFileLock(const fs::path &p)
{
    if constexpr (SystemType::IsWindows) {
        return boost::interprocess::file_lock(p.generic_wstring().c_str());
    } else {
        return boost::interprocess::file_lock(p.generic_u8string().c_str());
    }
}

static boost::interprocess::file_lock mkSingleLogicLock()
{
    const fs::path p = pathOfSingleProcLockFile();
    if (!fs::exists(p)) {
        std::ofstream ofs(p, std::ios::binary);
    }
    return mkFileLock(p);
}

bool Application::tryLockSingletonLogic()
{
    try {
        singletonLogicLock_ = mkSingleLogicLock();
        return singletonLogicLock_.try_lock();
    } catch (const std::exception &e) {
        LOGE << "make singleton logic file lock failed because: [" << e.what() << "]";
        try {
            singletonLogicLock_ = FileLock();
        } catch (const std::exception &e) {
            LOGE << "release singleton logic lock object failed because: [" << e.what() << "]";
        }
        return false;
    }
}

void Application::onNewConnection()
{
    while (serverSock_->hasPendingConnections()) {
        QLocalSocket *sessionSock = serverSock_->nextPendingConnection();
        
    }
}

}
