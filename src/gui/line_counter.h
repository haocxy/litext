#pragma once

#include <memory>

#include "core/sigconns.h"
#include "core/strand_pool.h"
#include "doc/declare_document.h"


namespace gui
{

namespace detail
{

class TextLayouterImpl : public std::enable_shared_from_this<TextLayouterImpl> {
public:
    TextLayouterImpl(StrandPool &pool, doc::Document &document);

    virtual ~TextLayouterImpl() {}

private:
    Strand &worker_;
    doc::Document &document_;
    SigConns sigConns_;
};

}

class TextLayouter {
public:
    TextLayouter(StrandPool &pool, doc::Document &document)
        : ptr_(std::make_shared<detail::TextLayouterImpl>(pool, document)) {

    }

    ~TextLayouter() {
    }

private:
    std::shared_ptr<detail::TextLayouterImpl> ptr_;
};




}
