#pragma once

#include <vector>

#include "singleton_server.h"


namespace gui
{

class SingletonServerImpl;


// 负责管理程序以单例模式执行时的服务端逻辑
// 主要是接收由客户端发来的打开文件的请求
// 还会定期向锁文件中写入服务端监听的端口
class SingletonServerOld : public SingletonServer {
public:
    struct StartInfo {
        fs::path serverRunningLock;
        fs::path infoFile;
        fs::path infoFileLock;
    };

    SingletonServerOld();

    virtual ~SingletonServerOld();

    virtual Signal<void()> &sigActivateUI() override;

    virtual Signal<void(const OpenInfos &)> &sigRecvOpenInfos() override;

    void start(const StartInfo &info);

private:
    SingletonServerImpl *impl_ = nullptr;
};

}
