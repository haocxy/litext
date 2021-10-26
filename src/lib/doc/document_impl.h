#pragma once

#include <atomic>
#include <shared_mutex>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/charset.h"
#include "core/time.h"
#include "core/async_deleter.h"

#include "doc_error.h"
#include "text_repo.h"
#include "text_loader.h"
#include "line_manger.h"
#include "row_cache.h"
#include "doc_define.h"
#include "doc_row.h"

#include "document.h"


namespace doc
{

class DocumentImpl {
public:
    DocumentImpl(StrandAllocator &strandAllocator, AsyncDeleter &asyncDeleter, const fs::path &file);

    ~DocumentImpl();

    const fs::path &path() const {
        return path_;
    }

    Signal<void(DocError)> &sigFatalError() {
        return sigFatalError_;
    }

    Signal<void(const Document::StartLoadEvent &)> &sigStartLoad() {
        return sigStartLoad_;
    }

    Signal<void(i64)> &sigFileSizeDetected() {
        return sigFileSizeDetected_;
    }

    Signal<void(Charset)> &sigCharsetDetected() {
        return sigCharsetDetected_;
    }

    Signal<void(const LoadProgress &)> &sigLoadProgress() {
        return sigLoadProgress_;
    }

    Signal<void()> &sigAllLoaded() {
        return sigAllLoaded_;
    }

    Signal<void(RowN nrows)> &sigRowCountUpdated() {
        return sigRowCountUpdated_;
    }

    RowN rowCnt() const {
        return lineManager_.rowCnt();
    }

    sptr<Row> rowAt(RowN row) const;

    std::map<RowN, sptr<Row>> rowsAt(const RowRange &range) const;

    void asyncGetLoadTimeUsageMs(std::function<void(i64 ms)> &&cb) {
        strand_.exec([this, cb] {
            cb(loadTimeUsage_.ms());
        });
    }

    void asyncLoad(Charset charset);

private:

    std::map<RowN, sptr<Row>> _rowsAt(const RowRange &range) const;

private:
    void _bindLoadSignals(SigConns &conns, TextLoader &loader);

    void _deleteLoader();

    void _asyncDeleteLoader();

private:
    Strand strand_;
    AsyncDeleter &asyncDeleter_;
    const fs::path path_;
    TextRepo textRepo_;
    LineManager lineManager_;
    mutable RowCache rowCache_;

    uptr<TextLoader> loader_;

    std::atomic_bool isInitLoad_{ true };
    std::atomic<i64> fileSize_{ 0 };
    std::atomic<Charset> charset_{ Charset::Unknown };
    SigConns loadSigConns_;
    SigConns lineSigConns_;

    Signal<void(DocError)> sigFatalError_;
    Signal<void(const Document::StartLoadEvent &)> sigStartLoad_;
    Signal<void(i64)> sigFileSizeDetected_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const LoadProgress &)> sigLoadProgress_;
    Signal<void()> sigAllLoaded_;
    Signal<void(RowN nrows)> sigRowCountUpdated_;

    TimeUsage<std::chrono::steady_clock> loadTimeUsage_;
};

}
