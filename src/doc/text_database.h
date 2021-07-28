#pragma once

#include <memory>
#include <fstream>

#include "core/fs.h"
#include "core/signal.h"
#include "core/charset.h"
#include "core/membuff.h"
#include "core/thread_pool.h"

#include "text_repo.h"
#include "part_loaded_event.h"


namespace doc
{

class TextLoader {
public:
    TextLoader(const fs::path &docPath);

    ~TextLoader();

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
    struct LoadingPartInfo {
        uintmax_t off = 0;
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
