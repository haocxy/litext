#pragma once

#include <memory>

#include "core/signal.h"
#include "core/strand_pool.h"
#include "doc/declare_document.h"


namespace gui
{

namespace detail
{

class LineCounter : public std::enable_shared_from_this<LineCounter> {
public:
    LineCounter(StrandPool &pool, doc::Document &document);

    virtual ~LineCounter() {}

private:
    Strand &worker_;
    doc::Document &document_;
    Slot slotPartLoaded_;
};

}

class LineCounter {
public:
    LineCounter(StrandPool &pool, doc::Document &document)
        : ptr_(std::make_shared<detail::LineCounter>(pool, document)) {}

    ~LineCounter() {}

private:
    std::shared_ptr<detail::LineCounter> ptr_;
};




}
