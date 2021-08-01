#pragma once

#include <atomic>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/charset.h"

#include "render_config.h"
#include "text_repo.h"
#include "text_loader.h"
#include "line_manger.h"
#include "doc_define.h"
#include "doc_row.h"


namespace doc
{

class Document {
public:
    Document(const fs::path &file);

    ~Document();

    void start(const RenderConfig &config);

    void setAreaSize(int w, int h);

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

    void loadRow(RowN row, std::function<void(std::shared_ptr<Row> row)> &&cb);

    void loadPage(RowN row, std::function<void(std::vector<std::shared_ptr<Row>> &&rows)> &&cb);

private:
    const fs::path path_;
    std::unique_ptr<RenderConfig> config_;
    TextRepo textRepo_;
    TextLoader loader_;
    LineManager lineManager_;
    std::atomic<Charset> charset_{ Charset::Unknown };
    SigConns sigConns_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;
    Signal<void(RowN nrows)> sigRowCountUpdated_;
};

}
