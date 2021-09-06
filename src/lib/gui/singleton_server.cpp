#include "singleton_server.h"

#include <memory>
#include <sstream>
#include <boost/dll.hpp>
#include <boost/asio.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

#include "core/logger.h"
#include "core/system.h"
#include "core/filelock.h"

#include "global/constants.h"


namespace
{

namespace asio = boost::asio;
namespace ip = asio::ip;
using tcp = ip::tcp;
using endpoint = tcp::endpoint;
namespace chrono = std::chrono;

static const chrono::seconds WriteInfoInterval(100);

namespace asio = boost::asio;

// 不要直接使用这个类
// 请使用SingletonServer类
class SingletonServerShareThis : public std::enable_shared_from_this<SingletonServerShareThis> {
public:
    using OpenInfos = gui::SingletonServer::OpenInfos;

    using StartInfo = gui::SingletonServer::StartInfo;

    SingletonServerShareThis()
        : acceptor_(context_, endpoint(tcp::v4(), 0))
        , timerForWriteInfo_(context_)
    {
        LOGD << "SingletonServer::SingletonServer() listening on port [" << port() << "]";
    }

    virtual ~SingletonServerShareThis()
    {
        if (started_) {
            context_.stop();
            stopping_ = true;
            thread_.join();
        }
    }

    Signal<void(const OpenInfos &)> &sigRecvOpenInfos() {
        return sigRecvOpenInfos_;
    }

    void start(const StartInfo &info)
    {
        if (started_) {
            return;
        }

        if (stopping_) {
            return;
        }

        serverRunningLockFile_ = info.serverRunningLock;
        infoFile_ = info.infoFile;
        infoFileLock_ = info.infoFileLock;

        serverRunningLock_ = FileLock(serverRunningLockFile_);
        serverRunningLockGuard_ = FileLockGuard(serverRunningLock_);

        started_ = true;

        initWrittenServerInfo();

        //startTimer();

        asyncAccept();

        thread_ = std::thread([this] {
            loop();
        });
    }

private:

    u16 port() const {
        return acceptor_.local_endpoint().port();
    }

    void loop()
    {
        while (!stopping_) {
            context_.run_one();
        }
    }

    void initWrittenServerInfo()
    {
        std::ostringstream ss;
        const boost::filesystem::path loc = boost::dll::program_location();
        const std::string utf8loc = fs::path(loc.generic_wstring()).generic_u8string();
        ss << port() << " " << utf8loc;

        writtenServerInfo_ = ss.str();

        writeServerInfo();
    }

    void startTimer()
    {
        asyncWaitTimerForWriteInfo();
    }

    void asyncWaitTimerForWriteInfo()
    {
        timerForWriteInfo_.expires_from_now(WriteInfoInterval);

        timerForWriteInfo_.async_wait([this](boost::system::error_code ec) {
            if (!ec) {
                writeServerInfo();
            } else if (ec == boost::asio::error::operation_aborted) {
                return;
            } else {
                std::ostringstream ss;
                ss << "SingletonServer::asyncWaitTimerForWriteInfo meet unknown error: ";
                ss << ec.message();
                throw std::logic_error(ss.str());
            }
            asyncWaitTimerForWriteInfo();
        });
    }

    void writeServerInfo()
    {
        LOGI << "SingletonServer::writeServerInfo() [" << writtenServerInfo_ << "]";

        FileLock lock(infoFileLock_);
        FileLockGuard guard(lock);

        {
            std::ofstream ofs(infoFile_, std::ios::binary);

            for (int i = 0; i < constants::ServerInfoRepeatCount; ++i) {
                ofs << writtenServerInfo_ << '\n';
            }
        }
    }

    void asyncAccept()
    {
        acceptor_.async_accept([this](const boost::system::error_code &ec, tcp::socket sock) {

        });
    }

private:
    asio::io_context context_;
    asio::ip::tcp::acceptor acceptor_;
    fs::path serverRunningLockFile_;
    FileLock serverRunningLock_;
    FileLockGuard serverRunningLockGuard_;
    fs::path infoFile_;
    fs::path infoFileLock_;
    std::string writtenServerInfo_;
    asio::steady_timer timerForWriteInfo_;
    Signal<void(const OpenInfos &)> sigRecvOpenInfos_;
    std::atomic_bool started_{ false };
    std::atomic_bool stopping_{ false };
    std::thread thread_;
};

}

namespace gui
{

class SingletonServerImpl {
public:
    SingletonServerImpl()
        : impl_(std::make_shared<SingletonServerShareThis>()) {}

    Signal<void(const SingletonServer::OpenInfos &)> &sigRecvOpenInfos() {
        return impl_->sigRecvOpenInfos();
    }

    void start(const SingletonServer::StartInfo &info) {
        impl_->start(info);
    }

private:
    std::shared_ptr<SingletonServerShareThis> impl_;
};

SingletonServer::SingletonServer()
    : impl_(new SingletonServerImpl)
{
}

SingletonServer::~SingletonServer()
{
    delete impl_;
    impl_ = nullptr;
}

Signal<void(const SingletonServer::OpenInfos &)> &SingletonServer::sigRecvOpenInfos()
{
    return impl_->sigRecvOpenInfos();
}

void SingletonServer::start(const StartInfo &info)
{
    return impl_->start(info);
}

}
