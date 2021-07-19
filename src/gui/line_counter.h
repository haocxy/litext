#pragma once

#include <memory>

#include "core/strand_pool.h"


namespace gui
{

namespace detail
{

class LineCounter : public std::enable_shared_from_this<LineCounter> {
public:
    LineCounter(StrandPool &pool);

    virtual ~LineCounter() {}

private:
    Strand &worker_;
};

}

class LineCounter {
public:
    LineCounter(StrandPool &pool)
        : ptr_(std::make_shared<detail::LineCounter>(pool)) {}

    ~LineCounter() {}

private:
    std::shared_ptr<detail::LineCounter> ptr_;
};




}
