#pragma once

#include <memory>

#include "core/signal.h"
#include "core/charset.h"
#include "core/strand_pool.h"
#include "doc/doc_define.h"
#include "text_database.h"


namespace doc
{

namespace detail
{

class Document : public std::enable_shared_from_this<Document> {
public:
    Document(StrandPool &pool, const fs::path &file, Strand &ownerThread);

    virtual ~Document();

    void init();

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
    Strand &ownerThread_;
    TextDatabase textDb_;
    Slot textDbSlotCharsetDetected_;
    Slot textDbSlotPartLoaded_;
    Slot textDbSlotAllLoaded_;
    Charset charset_ = Charset::Unknown;

    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;

};

}

class Document {
public:
    Document(StrandPool &pool, const fs::path &file, Strand &ownerThread)
        : ptr_(std::make_shared<detail::Document>(pool, file, ownerThread)) {

        ptr_->init();
    }

    virtual ~Document() {}

    Signal<void(Charset)> &sigCharsetDetected() {
        return ptr_->sigCharsetDetected();
    }

    Signal<void(const PartLoadedEvent &)> &sigPartLoaded() {
        return ptr_->sigPartLoaded();
    }

    Signal<void()> &sigAllLoaded() {
        return ptr_->sigAllLoaded();
    }

private:
    std::shared_ptr<detail::Document> ptr_;
};

}
