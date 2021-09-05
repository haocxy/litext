#include "singleton_server.h"

#include <sstream>
#include <boost/dll.hpp>

#include "core/logger.h"
#include "core/system.h"


namespace gui
{

namespace ip = asio::ip;
using tcp = ip::tcp;
using endpoint = tcp::endpoint;
namespace chrono = std::chrono;

static const chrono::seconds WriteInfoInterval(1);


SingletonServer::SingletonServer()
    : acceptor_(context_, endpoint(tcp::v4(), 0))
    , timerForWriteInfo_(context_)
{
    LOGD << "SingletonServer::SingletonServer() listening on port [" << port() << "]";

    
    // TODO 调试时临时直接在构造函数里启动,正式逻辑不要在这里启动
    start(SystemUtil::userHome() / LITEXT_WORKDIR_NAME / ".singleton.lock");
}

SingletonServer::~SingletonServer()
{
    if (started_) {
        context_.stop();
        stopping_ = true;
        thread_.join();
    }
}

void SingletonServer::start(const fs::path &lockFile)
{
    if (started_) {
        return;
    }

    if (stopping_) {
        return;
    }

    lockFile_ = lockFile;

    started_ = true;

    initWrittenServerInfo();

    startTimer();

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
    LOGD << "SingletonServer::writeServerInfo() [" << writtenServerInfo_ << "]";

    std::ofstream ofs(lockFile_, std::ios::binary);

    for (int i = 0; i < 3; ++i) {
        ofs << writtenServerInfo_ << '\n';
    }
}

}
