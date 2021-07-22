#pragma once

#include <memory>
#include <map>
#include <mutex>

#include "core/sigconns.h"
#include "core/strand_pool.h"
#include "doc/part_loaded_event.h"
#include "doc/declare_document.h"
#include "declare_text_area_config.h"


namespace gui
{

namespace detail
{

class TextLayouterImpl : public std::enable_shared_from_this<TextLayouterImpl> {
public:
    TextLayouterImpl(StrandPool &pool, const TextAreaConfig &config, int width, doc::Document &document);

    virtual ~TextLayouterImpl() {}

private:
    RowN countLines(const MemBuff &utf16data) const;

    void onPartLoaded(const doc::PartLoadedEvent &e);

private:
    Strand &worker_;
    const TextAreaConfig &config_;
    int width_ = 0;
    doc::Document &document_;
    SigConns sigConns_;

private:
    std::mutex mtxPartToRowCount_;
    std::map<int64_t, RowN> partToLineCount_;
};

}

class TextLayouter {
public:
    TextLayouter(StrandPool &pool, const TextAreaConfig &config, int width, doc::Document &document)
        : ptr_(std::make_shared<detail::TextLayouterImpl>(pool, config, width, document)) {

    }

    ~TextLayouter() {
    }

private:
    std::shared_ptr<detail::TextLayouterImpl> ptr_;
};




}
