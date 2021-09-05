#pragma once

#include <atomic>
#include <thread>
#include <vector>
#include <boost/asio.hpp>

#include "core/signal.h"
#include "doc/open_info.h"


namespace gui
{

namespace asio = boost::asio;


// 负责管理程序以单例模式执行时的服务端逻辑
// 主要是接收由客户端发来的打开文件的请求
// 还会定期向锁文件中写入服务端监听的端口
class SingletonServer {
public:
    using OpenInfos = std::vector<doc::OpenInfo>;

    SingletonServer();

    ~SingletonServer();

    Signal<void(const OpenInfos &)> &sigRecvOpenInfos() {
        return sigRecvOpenInfos_;
    }

    void start(const fs::path &lockFile);

    u16 port() const {
        return acceptor_.local_endpoint().port();
    }

private:
    void loop();

    void initWrittenServerInfo();

    void startTimer();

    void asyncWaitTimerForWriteInfo();

    void writeServerInfo();

private:
    asio::io_context context_;
    asio::ip::tcp::acceptor acceptor_;
    fs::path lockFile_;
    std::string writtenServerInfo_;
    asio::steady_timer timerForWriteInfo_;
    Signal<void(const OpenInfos &)> sigRecvOpenInfos_;
    std::atomic_bool started_{ false };
    std::atomic_bool stopping_{ false };
    std::thread thread_;
};

}

