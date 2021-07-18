#include "io_context.h"

#include <thread>
#include <atomic>

#include <boost/asio/io_context.hpp>


namespace detail
{

class IOContextImpl {
public:
    IOContextImpl() {
        thread_ = std::thread([this]{
            loop();
        });
    }

    ~IOContextImpl() {
        running_ = false;
        thread_.join();
    }

    void post(std::function<void()> &&f) {
        context_.post(std::move(f));
    }

    boost::asio::io_context &ioContext() {
        return context_;
    }

private:
    void loop() {
        while (running_) {
            context_.run();
        }
    }

private:
    std::atomic_bool running_{ true };
    std::thread thread_;
    boost::asio::io_context context_;
};

}

IOContext::IOContext()
{
    impl_ = new detail::IOContextImpl;
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
