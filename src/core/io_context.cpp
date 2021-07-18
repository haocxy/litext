#include "io_context.h"

#include <thread>
#include <atomic>

#include <boost/asio/io_context.hpp>

#ifdef WIN32
#include <Windows.h>
#endif



#include "logger.h"


static void setNameForCurrentThread(const std::string &name)
{
    if (name.empty()) {
        return;
    }
#ifdef WIN32
    std::wstring widestr;
    constexpr UINT page = CP_UTF8;
    constexpr DWORD flags = MB_PRECOMPOSED;
    const char *mbstr = name.c_str();
    const int mblen = static_cast<int>(name.size());
    const int wstrlen = MultiByteToWideChar(page, flags, mbstr, mblen, nullptr, 0);
    if (wstrlen <= 0) {
        std::ostringstream ss;
        ss << "MultiByteToWideChar() failed because [" << GetLastError() << "]";
        throw std::logic_error(ss.str());
    }
    std::wstring wstr;
    wstr.resize(wstrlen);
    const int n = MultiByteToWideChar(page, flags, mbstr, mblen, wstr.data(), static_cast<int>(wstr.size()));
    if (n <= 0) {
        std::ostringstream ss;
        ss << "MultiByteToWideChar() failed because [" << GetLastError() << "]";
        throw std::logic_error(ss.str());
    }

    SetThreadDescription(GetCurrentThread(), wstr.c_str());
#endif
}


namespace detail
{

static std::string makeName(const std::string &name) {
    std::ostringstream ss;
    ss << "IOContext(" << name << ")";
    return ss.str();
}

class IOContextImpl {
public:
    IOContextImpl(const std::string &name)
        : name_(makeName(name))
    {
        thread_ = std::thread([this]{
            setNameForCurrentThread(name_);
            loop();
        });
        LOGD << "IOContext [" << name_ << "] constructed";
    }

    ~IOContextImpl() {
        running_ = false;
        thread_.join();
        LOGD << "IOContext [" << name_ << "] destructed";
    }

    void post(std::function<void()> &&f) {
        context_.post(std::move(f));
    }

    boost::asio::io_context &ioContext() {
        return context_;
    }

private:
    void loop() {
        LOGI << "IOContext [" << name_ << "] loop start";
        while (running_) {
            context_.run();
        }
        LOGI << "IOContext [" << name_ << "] loop stop";
    }

private:
    const std::string name_;
    std::atomic_bool running_{ true };
    std::thread thread_;
    boost::asio::io_context context_;
};

}

IOContext::IOContext()
{
    impl_ = new detail::IOContextImpl(std::string());
}

IOContext::IOContext(const std::string &name)
{
    impl_ = new detail::IOContextImpl(name);
}

IOContext::~IOContext()
{
    delete impl_;
    impl_ = nullptr;
}

void IOContext::post(std::function<void()> &&f)
{
    impl_->post(std::move(f));
}

boost::asio::io_context &IOContext::ioContext()
{
    return impl_->ioContext();
}
