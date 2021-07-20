#pragma once


namespace boost::signals2
{
class connection;
}

namespace detail
{
class SigConnsImpl;
}

class SigConns {
public:
    SigConns();

    ~SigConns();

    SigConns &operator+=(boost::signals2::connection &&conn);

    void clear();

private:
    detail::SigConnsImpl *impl_ = nullptr;
};
