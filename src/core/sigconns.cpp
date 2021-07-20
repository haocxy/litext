#include "sigconns.h"

#include <boost/signals2/connection.hpp>


namespace detail
{

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
