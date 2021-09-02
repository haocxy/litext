#pragma once

#include <memory>

#include <boost/interprocess/sync/file_lock.hpp>

#include <QApplication>
#include <QLocalServer>


#include "core/basetype.h"


namespace gui::qt
{

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char *argv[]);

    virtual ~Application();

    // 是否以单例模式启动
    enum class SingletonMode {
        // 不以单例模式启动
        NoSingleton,
        // 尝试以单例模式启动, 如果失败则以非单例模式启动
        TrySingleton,
        // 必须以单例模式启动, 如果失败则推出程序
        MustSingleton,
    };

    int exec(SingletonMode singletonMode);

private:

    int execAsNonSingleton();

    int execAsServer(bool mustSingleton);

    int execAsClient(bool mustSingleton);

    // 尝试以单例方式启动的处理状态
    enum class SingletonState {

        Unknown,

        // 服务端启动失败
        ServerStartFailed,

        // 需要以单例模式启动, 且单例已启动
        ServerAlreadyStarted,

        // 需要以单例模式启动, 且单例未启动
        ServerNotStarted,
    };

    bool tryLockSingletonLogic();

private slots:
    void onNewConnection();

private:
    using FileLock = boost::interprocess::file_lock;
    FileLock singletonLogicLock_;
    QLocalServer *serverSock_ = nullptr;
    QLocalSocket *clientSock_ = nullptr;
};

}
