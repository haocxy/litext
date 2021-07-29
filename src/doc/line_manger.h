#pragma once

#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include <vector>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/task_queue.h"
#include "doc/part_loaded_event.h"
#include "doc/declare_document.h"
#include "gui/text_area_config.h"
#include "gui/cached_char_pix_width_provider.h"
#include "gui/row_walker.h"


namespace gui
{

class LineManager {
public:
    LineManager(const TextAreaConfig &config, int width, doc::Document &document);

    ~LineManager();

    Signal<void(RowN)> &sigRowCountUpdated() {
        return sigRowCountUpdated_;
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
        Worker(const font::FontIndex &fontIndex, TaskQueue<void(Worker &worker)> &taskQueue, const HLayoutConfig &config, int widthLimit);

        ~Worker();

        void stop() {
            stopping_ = true;
        }

        PartInfo countLines(const QString &content);

    private:
        void loop();

    private:
        TaskQueue<void(Worker &worker)> &taskQueue_;
        std::thread thread_;
        std::atomic_bool stopping_{ false };
        CachedCharPixWidthProvider widthProvider_;
        HLayoutConfig config_;
        int widthLimit_ = 0;
    };

    void onPartLoaded(const doc::PartLoadedEvent &e);

    RowN updatePartInfo(int64_t id, const PartInfo &newInfo);

private:
    TaskQueue<void(Worker &worker)> taskQueue_;
    std::vector<std::unique_ptr<Worker>> workers_;
    HLayoutConfig config_;
    doc::Document &document_;
    
    SigConns sigConns_;

    Signal<void(RowN)> sigRowCountUpdated_;

private:
    std::mutex mtxPartInfos_;
    std::map<int64_t, PartInfo> partIdToInfos_;
    RowN rowCount_ = 0;
    RowN lineCount_ = 0;
};

}

