#pragma once

#include <vector>

#include "core/signal.h"
#include "doc/open_info.h"


namespace gui
{

class SingletonServerImpl;


// 负责管理程序以单例模式执行时的服务端逻辑
// 主要是接收由客户端发来的打开文件的请求
// 还会定期向锁文件中写入服务端监听的端口
class SingletonServer {
public:
    using OpenInfos = std::vector<doc::OpenInfo>;

    struct StartInfo {
        fs::path serverRunningLock;
        fs::path infoFile;
        fs::path infoFileLock;
    };

    SingletonServer();

    ~SingletonServer();

    Signal<void()> &sigShowWindow();

    Signal<void(const OpenInfos &)> &sigRecvOpenInfos();

    void start(const StartInfo &info);

private:
    SingletonServerImpl *impl_ = nullptr;
};

}
