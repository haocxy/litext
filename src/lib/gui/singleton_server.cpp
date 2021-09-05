#include "singleton_server.h"

#include <sstream>
#include <boost/dll.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

#include "core/logger.h"
#include "core/system.h"
#include "core/filelock.h"


namespace gui
{

namespace ip = asio::ip;
using tcp = ip::tcp;
using endpoint = tcp::endpoint;
namespace chrono = std::chrono;

static const chrono::seconds WriteInfoInterval(100);


SingletonServer::SingletonServer()
    : acceptor_(context_, endpoint(tcp::v4(), 0))
    , timerForWriteInfo_(context_)
{
    LOGD << "SingletonServer::SingletonServer() listening on port [" << port() << "]";
}

SingletonServer::~SingletonServer()
{
    if (started_) {
        context_.stop();
        stopping_ = true;
        thread_.join();
    }
}

void SingletonServer::start(const StartInfo &info)
{
    if (started_) {
        return;
    }

    if (stopping_) {
        return;
    }

    singletonLogicLockFile_ = info.singletonLogicLockFile;
    infoFile_ = info.infoFile;
    infoFileLock_ = info.infoFileLock;

    singletonLogicLock_ = FileLock(singletonLogicLockFile_);
    singletonLogicLockGuard_ = FileLockGuard(singletonLogicLock_);

    started_ = true;

    initWrittenServerInfo();

    //startTimer();

    thread_ = std::thread([this] {
        loop();
    });
}

void SingletonServer::loop()
{
    while (!stopping_) {
        context_.run_one();
    }
}

void SingletonServer::initWrittenServerInfo()
{
    std::ostringstream ss;
    const boost::filesystem::path loc = boost::dll::program_location();
    const std::string utf8loc = fs::path(loc.generic_wstring()).generic_u8string();
    ss << port() << " " << utf8loc;

    writtenServerInfo_ = ss.str();

    writeServerInfo();
}

void SingletonServer::startTimer()
{
    asyncWaitTimerForWriteInfo();
}

void SingletonServer::asyncWaitTimerForWriteInfo()
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

void SingletonServer::writeServerInfo()
{
    LOGI << "SingletonServer::writeServerInfo() [" << writtenServerInfo_ << "]";

    FileLock lock(infoFileLock_);
    FileLockGuard guard(lock);

    {
        std::ofstream ofs(infoFile_, std::ios::binary);

        for (int i = 0; i < 3; ++i) {
            ofs << writtenServerInfo_ << '\n';
        }
    }
}

}
