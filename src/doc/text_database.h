#pragma once

#include <memory>
#include <fstream>

#include "core/fs.h"
#include "core/signal.h"
#include "core/charset.h"
#include "core/sqlite.h"
#include "core/membuff.h"
#include "core/strand_pool.h"

#include "part_loaded_event.h"


namespace doc
{

namespace detail
{

class TextDatabaseImpl : public std::enable_shared_from_this<TextDatabaseImpl> {
public:
    using Db = sqlite::Database;

    using Statement = sqlite::Statement;

    TextDatabaseImpl(const fs::path &docPath, StrandPool &pool);

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
    bool prepareDatabase();

    bool prepareSaveDataStatement();

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
    const fs::path dbPath_;
    std::ifstream ifs_;
    Strand &worker_;
    Db db_;
    Statement saveDataStmt_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;
};

}

class TextDatabase {
public:
    TextDatabase(const fs::path &docPath, StrandPool &pool)
        : impl_(std::make_shared<detail::TextDatabaseImpl>(docPath, pool)) {
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
