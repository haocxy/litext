#pragma once

#include <atomic>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/charset.h"
#include "doc/doc_define.h"
#include "text_loader.h"


namespace doc
{

class Document {
public:
    Document(const fs::path &file);

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

private:
    const fs::path path_;
    TextLoader loader_;
    std::atomic<Charset> charset_{ Charset::Unknown };
    SigConns sigConns_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;
};

}
