#pragma once

#include <optional>

#include "core/fs.h"
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
    Document(AsyncDeleter &asyncDeleter, const fs::path &file);

    ~Document();

    const fs::path &path() const;

    i64 loadTimeUsageMs() const;

    void load();

    Signal<void(DocError)> &sigFatalError();

    Signal<void(i64)> &sigFileSizeDetected();

    Signal<void(Charset)> &sigCharsetDetected();

    Signal<void(const LoadProgress &)> &sigLoadProgress();

    Signal<void()> &sigAllLoaded();

    Signal<void(RowN)> &sigRowCountUpdated();

    RowN rowCnt() const;

    sptr<Row> rowAt(RowN row) const;

    std::map<RowN, sptr<Row>> rowsAt(const RowRange &range) const;

private:
    doc::DocumentImpl *impl_ = nullptr;
};

}
