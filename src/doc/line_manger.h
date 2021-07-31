#pragma once

#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include <vector>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/thread.h"
#include "core/font_index.h"
#include "part_loaded_event.h"
#include "declare_document.h"
#include "text_loader.h"
#include "glyph_width_cache.h"
#include "render_config.h"
#include "row_walker.h"


namespace doc
{

class LineManager {
public:
    LineManager(TextLoader &loader);

    ~LineManager();

    void updateConfig(const RenderConfig &config);

    Signal<void(const PartLoadedEvent &)> &sigPartLoaded() {
        return sigPartLoaded_;
    }

    Signal<void(RowN)> &sigRowCountUpdated() {
        return sigRowCountUpdated_;
    }

    RowN rowCount() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return rowCount_;
    }

private:

    struct PartInfo {
        PartInfo() {}
        PartInfo(RowN rowCount, RowN lineCount) : rowCount(rowCount), lineCount(lineCount) {}
        RowN rowCount = 0;
        RowN lineCount = 0;
    };

    class Worker {
    public:
        Worker(TaskQueue<void(Worker &worker)> &taskQueue);

        ~Worker();

        void stop() {
            stopping_ = true;
        }

        void updateConfig(const RenderConfig &config);

        PartInfo countLines(const QString &content);

    private:
        void loop();

    private:
        TaskQueue<void(Worker &worker)> &taskQueue_;
        std::thread thread_;
        std::atomic_bool stopping_{ false };
        std::unique_ptr<RenderConfig> config_;
        std::unique_ptr<GlyphWidthCache> widthProvider_;
    };

    void onPartLoaded(const doc::PartLoadedEvent &e);

    RowN updatePartInfo(int64_t id, const PartInfo &newInfo);

private:
    TextLoader &loader_;
    RenderConfig config_;
    TaskQueue<void(Worker &worker)> taskQueue_;
    std::vector<std::unique_ptr<Worker>> workers_;
    
    SigConns sigConns_;
    Signal<void(RowN)> sigRowCountUpdated_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;

private:
    mutable std::mutex mtx_;
    std::map<int64_t, PartInfo> partIdToInfos_;
    RowN rowCount_ = 0;
    RowN lineCount_ = 0;
};

}

