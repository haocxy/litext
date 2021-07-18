#include "io_context.h"

#include <thread>
#include <atomic>

#include <boost/asio/io_context.hpp>

#include "logger.h"


namespace detail
{

class IOContextImpl {
public:
    IOContextImpl(const std::string &name) {
        thread_ = std::thread([this]{
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
