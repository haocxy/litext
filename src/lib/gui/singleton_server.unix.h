#pragma once

#include <vector>

#include "singleton_server.h"


namespace gui
{

class SingletonServerUnixImpl;


// 负责管理程序以单例模式执行时的服务端逻辑
// 主要是接收由客户端发来的打开文件的请求
// 还会定期向锁文件中写入服务端监听的端口
class SingletonServerUnix : public SingletonServer {
public:
    SingletonServerUnix();

    virtual ~SingletonServerUnix();

    virtual Signal<void()> &sigActivateUI() override;

    virtual Signal<void(const OpenInfos &)> &sigRecvOpenInfos() override;

    virtual void start(const StartInfo &info) override;

private:
    SingletonServerUnixImpl *impl_ = nullptr;
};

}
