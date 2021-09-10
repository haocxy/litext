#include "singleton_server.h"

#include <memory>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

#include "core/endian.h"
#include "core/logger.h"
#include "core/system.h"
#include "core/filelock.h"

#include "global/msg.h"
#include "global/constants.h"
#include "global/server_info.h"


namespace
{

namespace asio = boost::asio;
namespace ip = asio::ip;
using tcp = ip::tcp;
using endpoint = tcp::endpoint;
namespace chrono = std::chrono;

static const chrono::seconds WriteInfoInterval(100);


// 不要直接使用这个类
// 请使用SingletonServer类
class SingletonServerShareThis : public std::enable_shared_from_this<SingletonServerShareThis> {
public:

    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(std::shared_ptr<SingletonServerShareThis> server, tcp::socket &&sock)
            : server_(server), sock_(std::move(sock)) {}

    public:
        void start() {
            asyncReadDataLen();
        }

    private:
        void asyncReadDataLen() {
            auto self(shared_from_this());
            asio::mutable_buffer buff(&recvLenNetEndian_, sizeof(recvLenNetEndian_));
            asio::async_read(sock_, buff, [self, this](boost::system::error_code ec, std::size_t nbytes) {
                if (ec) {
                    throw std::logic_error(ec.message());
                }
                const i64 recvLen = endian::netToNative(recvLenNetEndian_);
                asyncReadData(recvLen);
            });
        }

        void asyncReadData(i64 len) {
            recvData_.resize(len);
            asio::mutable_buffer buff(recvData_.data(), recvData_.size());
            auto self(shared_from_this());
            asio::async_read(sock_, buff, [self, this](boost::system::error_code ec, std::size_t nbytes) {
                if (ec) {
                    throw std::logic_error(ec.message());
                }
                const msg::Pack pack = msg::Pack::deserialize(recvData_);
                sendData_ = server_->handleMsg(pack).serialize();
                asyncWriteLen();
            });
        }

        void asyncWriteLen() {
            sendLenNetEndian_ = endian::nativeToNet<i64>(sendData_.size());
            asio::const_buffer buff(&sendLenNetEndian_, sizeof(sendLenNetEndian_));
            auto self(shared_from_this());
            asio::async_write(sock_, buff, [self, this](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    throw std::logic_error(ec.message());
                }
                asyncWriteData();
            });
        }

        void asyncWriteData() {
            asio::const_buffer buff(sendData_.data(), sendData_.size());
            auto self(shared_from_this());
            asio::async_write(sock_, buff, [self, this](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    throw std::logic_error(ec.message());
                }
                sock_.close();
            });
        }

    private:
        std::shared_ptr<SingletonServerShareThis> server_;
        tcp::socket sock_;
        i64 recvLenNetEndian_ = 0;
        std::string recvData_;
        i64 sendLenNetEndian_ = 0;
        std::string sendData_;
    };

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

    Signal<void()> &sigShowWindow() {
        return sigShowWindow_;
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

    msg::Pack handleMsg(const msg::Pack &pack)
    {
        if (pack.is<msg::ShowWindow>()) {
            sigShowWindow_();
        } else if (pack.is<msg::OpenFiles>()) {
            sigRecvOpenInfos_(toOpenInfos(pack));
        }
        return msg::Ok();
    }

    static OpenInfos toOpenInfos(const msg::Pack &pack) {
        OpenInfos openInfos;
        msg::OpenFiles openFilesMsg = pack.unpack();
        for (const msg::OpenFiles::OpenInfo &info : openFilesMsg.files) {
            u32str u32s = info.u8file;
            openInfos.push_back({ fs::path(static_cast<std::u32string &&>(u32s)), info.row });
        }
        return openInfos;
    }

private:

    u16 port() const {
        return acceptor_.local_endpoint().port();
    }

    void loop()
    {
        while (!stopping_) {
            context_.run();
            LOGI << "after context_.run()";
        }
    }

    void initWrittenServerInfo()
    {
        const fs::path exe = SystemUtil::exePath();
        writtenServerInfo_ = ServerInfo(port(), exe).toLine();

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

        try {

            ScopedFileLock lock(infoFileLock_);

            std::ofstream ofs(infoFile_);

            for (int i = 0; i < constants::ServerInfoRepeatCount; ++i) {
                ofs << writtenServerInfo_ << std::endl;
            }

        } catch (const std::exception &e) {
            std::string msg = e.what();
            LOGE << "writeServerInfo meet exception, what(): " << e.what();
            throw;
        }
    }

    void asyncAccept()
    {
        acceptor_.async_accept([this](const boost::system::error_code &ec, tcp::socket sock) {
            std::make_shared<Session>(shared_from_this(), std::move(sock))->start();
            asyncAccept();
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
    Signal<void()> sigShowWindow_;
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

    Signal<void()> &sigShowWindow() {
        return impl_->sigShowWindow();
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

Signal<void()> &SingletonServer::sigShowWindow()
{
    return impl_->sigShowWindow();
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
