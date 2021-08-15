#pragma once

#include "core/fs.h"
#include "core/signal.h"
#include "core/charset.h"

#include "render_config.h"
#include "part_loaded_event.h"
#include "doc_row.h"


namespace doc
{

class DocumentImpl;

class Document {
public:
    Document(const fs::path &file);

    ~Document();

    void start(const RenderConfig &config);

    void setAreaSize(int w, int h);

    Signal<void(Charset)> &sigCharsetDetected();

    Signal<void(const PartLoadedEvent &)> &sigPartLoaded();

    Signal<void()> &sigAllLoaded();

    Signal<void(RowN)> &sigRowCountUpdated();

    RowN rowCnt() const;

    void loadRow(RowN row, std::function<void(std::shared_ptr<Row> row)> &&cb);

    void loadPage(RowN row, std::function<void(std::vector<std::shared_ptr<Row>> &&rows)> &&cb);

private:
    doc::DocumentImpl *impl_ = nullptr;
};

}
