#pragma once

#include <optional>

#include "core/fs.h"
#include "core/signal.h"
#include "core/charset.h"

#include "doc_row.h"
#include "render_config.h"
#include "part_loaded_event.h"


namespace doc
{

class DocumentImpl;

class Document {
public:
    Document(const fs::path &file);

    ~Document();

    void start();

    Signal<void(Charset)> &sigCharsetDetected();

    Signal<void(const PartLoadedEvent &)> &sigPartLoaded();

    Signal<void()> &sigAllLoaded();

    Signal<void(RowN)> &sigRowCountUpdated();

    RowN rowCnt() const;

    uptr<Row> rowAt(RowN row);

    std::map<RowN, uptr<Row>> rowsAt(const RowRange &range);

private:
    doc::DocumentImpl *impl_ = nullptr;
};

}
