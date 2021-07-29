#pragma once

#include <atomic>
#include <memory>
#include <fstream>
#include <thread>

#include "core/fs.h"
#include "core/signal.h"
#include "core/charset.h"
#include "core/membuff.h"
#include "core/thread.h"

#include "text_repo.h"
#include "part_loaded_event.h"


namespace doc
{

class TextLoader {
public:
    TextLoader(const fs::path &docPath);

    ~TextLoader();

    void loadAll();

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

    struct LoadingPart {
        i64 off = 0;
        i64 filesize = 0;
        Charset charset = Charset::Unknown;
        MemBuff data;
    };

    using LoadingParts = BlockQueue<LoadingPart>;

    class Reader {
    public:
        Reader(const fs::path &docPath, TaskQueue<void(Reader &)> &tasks, LoadingParts &loadingParts);

        ~Reader();

        void readAll();

    private:
        void loop();

    private:
        const fs::path docPath_;
        TaskQueue<void(Reader &)> &tasks_;
        LoadingParts &loadingParts_;
        
        std::thread th_;
        std::atomic_bool stopping_{ false };
    };

    class Decoder {
    public:
        Decoder(TextLoader &self, LoadingParts &loadingParts);

        ~Decoder();

    private:
        void loop();

        void decodePart(LoadingPart &&part);

    private:
        TextLoader &self_;
        BlockQueue<LoadingPart> &loadingParts_;
        TextRepo::SavePartStmt stmtSavePart_;
        std::thread th_;
        std::atomic_bool stopping_{ false };
    };

private:
    TextRepo textRepo_;
    TaskQueue<void(Reader &)> readerTasks_;
    LoadingParts loadingParts_;
    std::unique_ptr<Reader> reader_;
    std::vector<std::unique_ptr<Decoder>> decoders_;
    
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;
};

}
