#pragma once

#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include <vector>

#include "core/sigconns.h"
#include "core/block_queue.h"
#include "core/thread_pool.h"
#include "doc/part_loaded_event.h"
#include "doc/declare_document.h"
#include "text_area_config.h"
#include "cached_char_pix_width_provider.h"
#include "row_walker.h"


namespace gui
{

namespace detail
{

class TextLayouterImpl : public std::enable_shared_from_this<TextLayouterImpl> {
public:
    TextLayouterImpl(const TextAreaConfig &config, int width, doc::Document &document);

    virtual ~TextLayouterImpl();

private:
    void onPartLoaded(const doc::PartLoadedEvent &e);

private:

    class TextLayouterWorker {
    public:
        TextLayouterWorker(const font::FontIndex &fontIndex, BlockQueue<std::function<void(TextLayouterWorker &worker)>> &taskQueue, const NewRowWalker::Config &config);

        ~TextLayouterWorker();

        void stop() {
            stopping_ = true;
        }

        RowN countLines(const MemBuff &utf16data);

    private:
        void loop();

    private:
        BlockQueue<std::function<void(TextLayouterWorker &worker)>> &taskQueue_;
        std::thread thread_;
        std::atomic_bool stopping_{ false };
        CachedCharPixWidthProvider widthProvider_;
        NewRowWalker::Config config_;
    };

private:
    BlockQueue<std::function<void(TextLayouterWorker &worker)>> taskQueue_;
    std::vector<std::unique_ptr<TextLayouterWorker>> workers_;
    ThreadPool worker_;
    const TextAreaConfig config_;
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
    TextLayouter(const TextAreaConfig &config, int width, doc::Document &document)
        : ptr_(std::make_shared<detail::TextLayouterImpl>(config, width, document)) {

    }

    ~TextLayouter() {
    }

private:
    std::shared_ptr<detail::TextLayouterImpl> ptr_;
};




}
