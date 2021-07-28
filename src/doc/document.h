#pragma once

#include <atomic>
#include <memory>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/charset.h"
#include "core/thread_pool.h"
#include "doc/doc_define.h"
#include "text_database.h"


namespace doc
{

namespace detail
{

class Document : public std::enable_shared_from_this<Document> {
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
    ThreadPool worker_{ "Document", 1 };
    TextLoader loader_;
    std::atomic<Charset> charset_{ Charset::Unknown };
    SigConns sigConns_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;
};

}

class Document {
public:
    Document(const fs::path &file)
        : ptr_(std::make_shared<detail::Document>(file)) {

    }

    ~Document() {
    }

    void start() {
        ptr_->start();
    }

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
