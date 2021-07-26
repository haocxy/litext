#pragma once

#include <memory>
#include <map>
#include <mutex>

#include "core/sigconns.h"
#include "core/thread_pool.h"
#include "doc/part_loaded_event.h"
#include "doc/declare_document.h"
#include "text_area_config.h"
#include "cached_char_pix_width_provider.h"


namespace gui
{

namespace detail
{

class TextLayouterImpl : public std::enable_shared_from_this<TextLayouterImpl> {
public:
    TextLayouterImpl(const TextAreaConfig &config, int width, doc::Document &document);

    virtual ~TextLayouterImpl() {}

private:
    RowN countLines(const MemBuff &utf16data);

    void onPartLoaded(const doc::PartLoadedEvent &e);

private:
    ThreadPool worker_{ "TextLayouter", 1 };
    const TextAreaConfig config_;
    int width_ = 0;
    doc::Document &document_;
    CachedCharPixWidthProvider widthProvider_;
    SigConns sigConns_;

private:
    std::mutex mtxPartToRowCount_;
    std::map<int64_t, RowN> partToLineCount_;
};

}

class TextLayouter {
public:
    TextLayouter(const TextAreaConfig &config, int width, doc::Document &document)
        : ptr_(std::make_shared<detail::TextLayouterImpl>(config, width, document)) {

    }

    ~TextLayouter() {
    }

private:
    std::shared_ptr<detail::TextLayouterImpl> ptr_;
};




}
