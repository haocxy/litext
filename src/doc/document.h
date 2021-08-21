#pragma once

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

    void loadRow(RowN row, std::function<void(std::shared_ptr<Row>)> &&cb);

    void loadRows(const RowRange &range, std::function<void(std::vector<std::shared_ptr<Row>> &&rows)> &&cb);

private:
    doc::DocumentImpl *impl_ = nullptr;
};

}
