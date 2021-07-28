#pragma once

#include <memory>
#include <fstream>

#include "core/fs.h"
#include "core/signal.h"
#include "core/charset.h"
#include "core/sqlite.h"
#include "core/membuff.h"
#include "core/thread_pool.h"

#include "text_repo.h"
#include "part_loaded_event.h"


namespace doc
{

namespace detail
{

class TextDatabaseImpl : public std::enable_shared_from_this<TextDatabaseImpl> {
public:
    using Db = sqlite::Database;

    using Statement = sqlite::Statement;

    TextDatabaseImpl(const fs::path &docPath);

    virtual ~TextDatabaseImpl();

    void start();

    void stop();

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
    struct LoadingPartInfo {
        uintmax_t off = 0;
        uintmax_t len = 0;
        Charset charset = Charset::Unknown;
    };

    void loadPart(const MemBuff &readBuff, const LoadingPartInfo &info);

    void loadAll();

    void asyncLoadAll();

    

private:
    const fs::path docPath_;
    std::ifstream ifs_;
    ThreadPool worker_{"TextDatabase", 1};
    TextRepo textRepo_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;
};

}

class TextDatabase {
public:
    TextDatabase(const fs::path &docPath)
        : impl_(std::make_shared<detail::TextDatabaseImpl>(docPath)) {
    }

    ~TextDatabase() {
        impl_->stop();
    }

    void start() {
        impl_->start();
    }

    Signal<void(Charset)> &sigCharsetDetected() {
        return impl_->sigCharsetDetected();
    }

    Signal<void(const PartLoadedEvent &)> &sigPartLoaded() {
        return impl_->sigPartLoaded();
    }

    Signal<void()> &sigAllLoaded() {
        return impl_->sigAllLoaded();
    }

private:
    std::shared_ptr<detail::TextDatabaseImpl> impl_;
};


}
