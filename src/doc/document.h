#pragma once

#include "core/signal.h"
#include "core/charset.h"
#include "core/strand_pool.h"
#include "doc/doc_define.h"
#include "text_database.h"


namespace doc
{

class Document {
public:
    Document(StrandPool &pool, const fs::path &file, Strand &ownerThread);

    ~Document();

    Signal<void()> &sigAllLoaded() {
        return sigAllLoaded_;
    }

private:
    const fs::path path_;
    Strand &ownerThread_;
    TextDatabase textDb_;

    Signal<void()> sigAllLoaded_;
    Slot slotTextDatabaseAllLoaded_;
};

}
