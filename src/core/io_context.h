#pragma once

#include <string>
#include <functional>


namespace detail
{
class IOContextImpl;
}

namespace boost::asio
{
class io_context;
}

class IOContext {
public:
    IOContext();

    IOContext(const std::string &name);

    ~IOContext();

    void post(std::function<void()> &&f);

    boost::asio::io_context &ioContext();

private:
    detail::IOContextImpl *impl_ = nullptr;
};
