#pragma once

#include <atomic>
#include <mutex>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/charset.h"
#include "core/time.h"

#include "doc_error.h"
#include "text_repo.h"
#include "text_loader.h"
#include "line_manger.h"
#include "row_cache.h"
#include "doc_define.h"
#include "doc_row.h"


namespace doc
{

class DocumentImpl {
public:
    DocumentImpl(const fs::path &file);

    ~DocumentImpl();

    const fs::path &path() const {
        return path_;
    }

    i64 loadTimeUsageMs() const {
        return loadTimeUsage_.ms();
    }

    void start();

    Signal<void(DocError)> &sigFatalError() {
        return sigFatalError_;
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

private:
    const fs::path path_;
    TextRepo textRepo_;
    TextLoader loader_;
    LineManager lineManager_;
    mutable RowCache rowCache_;
    std::atomic<i64> fileSize_{ 0 };
    std::atomic<Charset> charset_{ Charset::Unknown };
    SigConns sigConns_;

    Signal<void(DocError)> sigFatalError_;
    Signal<void(i64)> sigFileSizeDetected_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const LoadProgress &)> sigLoadProgress_;
    Signal<void()> sigAllLoaded_;
    Signal<void(RowN nrows)> sigRowCountUpdated_;

    TimeUsage<std::chrono::steady_clock> loadTimeUsage_;
};

}
