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

#include "doc_error.h"
#include "decoded_part.h"
#include "doc_part.h"


namespace doc
{

class TextLoader {
public:
    TextLoader(const fs::path &docPath);

    ~TextLoader();

    void loadAll(Charset charset);

    Signal<void(DocError)> &sigFatalError() {
        return sigFatalError_;
    }

    Signal<void(i64)> &sigFileSizeDetected() {
        return sigFileSizeDetected_;
    }

    Signal<void(Charset)> &sigCharsetDetected() {
        return sigCharsetDetected_;
    }

    Signal<void(const DocPart &)> &sigPartLoaded() {
        return sigPartLoaded_;
    }

    Signal<void()> &sigAllLoaded() {
        return sigAllLoaded_;
    }

private:

    struct LoadingPart {
        i64 off = 0;
        Charset charset = Charset::Unknown;
        bool isLast = false;
        MemBuff data;
    };

    using LoadingParts = BlockQueue<LoadingPart>;

    class Reader {
    public:
        Reader(TextLoader &self, const fs::path &docPath, TaskQueue<void(Reader &)> &tasks, LoadingParts &loadingParts);

        ~Reader();

        void readAll(Charset charset);

    private:
        void loop();

    private:
        TextLoader &self_;
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
        std::thread th_;
        std::atomic_bool stopping_{ false };
    };

    Charset updateCharset(const MemBuff &data);

    void specifyCharsetIfValid(Charset charset);

private:
    TaskQueue<void(Reader &)> readerTasks_;
    LoadingParts loadingParts_;
    uptr<Reader> reader_;
    std::vector<uptr<Decoder>> decoders_;

    Signal<void(DocError)> sigFatalError_;
    Signal<void(i64)> sigFileSizeDetected_;
    Signal<void(Charset)> sigCharsetDetected_;
    Signal<void(const DocPart &)> sigPartLoaded_;
    Signal<void()> sigAllLoaded_;

    using Mtx = std::recursive_mutex;
    using Lock = std::unique_lock<Mtx>;
    mutable Mtx mtx_;
    Charset detectedCharset_{Charset::Unknown};
    opt<Charset> specifiedCharset_;
};

}
