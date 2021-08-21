#pragma once

#include <atomic>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/charset.h"

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

    void start();

    Signal<void(DocError)> &sigFatalError() {
        return sigFatalError_;
    }

    Signal<void(Charset)> &sigCharsetDetected() {
        return sigCharsetDetected_;
    }

    Signal<void(const PartLoadedEvent &)> &sigPartLoaded() {
        return sigPartLoaded_;
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

    uptr<Row> rowAt(RowN row);

    std::map<RowN, uptr<Row>> rowsAt(const RowRange &range);

private:
    const fs::path path_;
    TextRepo textRepo_;
    TextLoader loader_;
    LineManager lineManager_;
    RowCache rowCache_;
    std::atomic<Charset> charset_{ Charset::Unknown };
    SigConns sigConns_;

    Signal<void(DocError)> sigFatalError_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;
    Signal<void(RowN nrows)> sigRowCountUpdated_;
};

}
