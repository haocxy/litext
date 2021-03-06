#pragma once

#include <optional>
#include <shared_mutex>

#include "core/fs.h"
#include "core/thread.h"
#include "core/signal.h"
#include "core/charset.h"
#include "core/async_deleter.h"

#include "doc_error.h"
#include "doc_row.h"
#include "load_progress.h"


namespace doc
{

class DocumentImpl;

class Document {
public:
    Document(StrandAllocator &strandAllocator, AsyncDeleter &asyncDeleter, const fs::path &file);

    ~Document();

    const fs::path &path() const;

    void asyncGetLoadTimeUsageMs(std::function<void(i64 ms)> &&cb);

    void asyncLoad(Charset charset);

    Signal<void(DocError)> &sigFatalError();

    class StartLoadEvent {
    public:
        StartLoadEvent() {}

        explicit StartLoadEvent(bool isInitLoad)
            : isInitLoad_(isInitLoad) {}

        bool isInitLoad() const {
            return isInitLoad_;
        }

    private:
        bool isInitLoad_ = false;
    };

    Signal<void(const StartLoadEvent &)> &sigStartLoad();

    Signal<void(i64)> &sigFileSizeDetected();

    Signal<void(Charset)> &sigCharsetDetected();

    Signal<void(const LoadProgress &)> &sigLoadProgress();

    Signal<void()> &sigAllLoaded();

    Signal<void(RowN)> &sigRowCountUpdated();

    RowN rowCnt() const;

    sptr<Row> rowAt(RowN row) const;

    std::map<RowN, sptr<Row>> rowsAt(const RowRange &range) const;

private:
    using Mtx = std::shared_mutex;
    using ReadLock = std::shared_lock<Mtx>;
    using WriteLock = std::lock_guard<Mtx>;
    mutable Mtx mtx_;
    doc::DocumentImpl *impl_ = nullptr;
};

}
