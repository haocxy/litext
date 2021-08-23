#include "sigconns.h"

#include <boost/signals2/connection.hpp>


namespace detail
{

class SigConnImpl {
public:
    SigConnImpl() {}

    ~SigConnImpl() {
        conn_.disconnect();
    }

    void set(boost::signals2::connection &conn) {
        conn_ = std::move(conn);
    }

    void disconnect() {
        conn_.disconnect();
    }

private:
    boost::signals2::connection conn_;
};

class SigConnsImpl {
public:
    SigConnsImpl() {}

    ~SigConnsImpl() {
        for (boost::signals2::connection &conn : conns_) {
            conn.disconnect();
        }
    }

    void add(boost::signals2::connection &&conn) {
        conns_.push_back(std::move(conn));
    }

    void clear() {
        for (boost::signals2::connection &conn : conns_) {
            conn.disconnect();
        }
        conns_.clear();
    }

private:
    std::vector<boost::signals2::connection> conns_;
};

}

SigConns::SigConns()
    : impl_(new detail::SigConnsImpl)
{
}

SigConns::~SigConns()
{
    delete impl_;
    impl_ = nullptr;
}

SigConns &SigConns::operator+=(boost::signals2::connection &&conn)
{
    impl_->add(std::move(conn));
    return *this;
}

void SigConns::clear() {
    impl_->clear();
}

SigConn::SigConn()
    : impl_(new detail::SigConnImpl)
{
}

SigConn::~SigConn()
{
    delete impl_;
    impl_ = nullptr;
}

SigConn &SigConn::operator=(boost::signals2::connection &&conn)
{
    impl_->set(std::move(conn));
    return *this;
}

void SigConn::disconnect()
{
    impl_->disconnect();
}




