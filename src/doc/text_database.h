#pragma once

#include <memory>
#include <fstream>

#include "core/fs.h"
#include "core/charset.h"
#include "core/sqlite.h"
#include "core/membuff.h"
#include "core/io_context_strand.h"


namespace doc
{

namespace detail
{

class TextDatabaseImpl : public std::enable_shared_from_this<TextDatabaseImpl> {
public:
    using Db = sqlite::Database;

    using Statement = sqlite::Statement;

    TextDatabaseImpl(const fs::path &docPath, IOContextStrand::Pool &pool);

    virtual ~TextDatabaseImpl();

    void start();

private:
    bool prepareDatabase();

    bool prepareSaveDataStatement();

    struct LoadingPartInfo {
        uintmax_t off = 0;
        uintmax_t len = 0;
        Charset charset = Charset::Unknown;
    };

    void loadPart(const MemBuff &readBuff, MemBuff &decodeBuff, const LoadingPartInfo &info);

    void loadAll();

    void asyncLoadAll();

private:
    const fs::path docPath_;
    const fs::path dbPath_;
    std::ifstream ifs_;
    IOContextStrand worker_;
    Db db_;
    Statement saveDataStmt_;
};

}

class TextDatabase {
public:
    TextDatabase(const fs::path &docPath, IOContextStrand::Pool &pool) : impl_(std::make_shared<detail::TextDatabaseImpl>(docPath, pool)) {
        impl_->start();
    }

    ~TextDatabase() {}

private:
    std::shared_ptr<detail::TextDatabaseImpl> impl_;
};


}
