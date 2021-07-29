#pragma once

#include <atomic>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/charset.h"
#include "doc/doc_define.h"

#include "render_config.h"
#include "text_loader.h"
#include "line_manger.h"



namespace doc
{

class Document {
public:
    Document(const fs::path &file, const RenderConfig &config);

    virtual ~Document();

    void start();

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

private:
    const fs::path path_;
    RenderConfig config_;
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
