#pragma once


namespace boost::signals2
{
class connection;
}

namespace detail
{
class SigConnImpl;
class SigConnsImpl;
}

class SigConn {
public:
    SigConn();

    ~SigConn();

    SigConn &operator=(boost::signals2::connection &&conn);

    void disconnect();

private:
    detail::SigConnImpl *impl_ = nullptr;
};

class SigConns {
public:
    SigConns();

    ~SigConns();

    SigConns &operator+=(boost::signals2::connection &&conn);

    void clear();

private:
    detail::SigConnsImpl *impl_ = nullptr;
};
