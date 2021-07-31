#pragma once

#include <atomic>
#include <memory>
#include <map>
#include <set>
#include <mutex>
#include <vector>

#include "core/signal.h"
#include "core/sigconns.h"
#include "core/thread.h"
#include "core/font_index.h"
#include "part_loaded_event.h"
#include "declare_document.h"
#include "text_repo.h"
#include "text_loader.h"
#include "glyph_width_cache.h"
#include "render_config.h"
#include "row_walker.h"


namespace doc
{

class LineManager {
public:
    LineManager(TextRepo &textRespo, TextLoader &loader);

    ~LineManager();

    void init(const RenderConfig &config);

    Signal<void(const PartLoadedEvent &)> &sigPartLoaded() {
        return sigPartLoaded_;
    }

    Signal<void(RowN)> &sigRowCountUpdated() {
        return sigRowCountUpdated_;
    }

    RowN rowCnt() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return rowCount_;
    }

    void loadRow(RowN row, std::function<void(QString s)> &&cb);

private:

    struct PartInfo {
        PartInfo() {}
        PartInfo(RowN rowCount, RowN lineCount)
            : rowCount(rowCount), lineCount(lineCount) {}

        i64 id = 0;
        i64 byteOffset = 0;
        i64 nbytes = 0;
        RowN rowCount = 0;
        RowN lineCount = 0;
    };

    class Worker {
    public:
        Worker(TextRepo &textRepo, TaskQueue<void(Worker &worker)> &taskQueue);

        ~Worker();

        void stop() {
            stopping_ = true;
        }

        void init(const RenderConfig &config);

        void setWidthLimit(int w);

        PartInfo countLines(const QString &s);

        i64 savePart(i64 off, i64 nrows, i64 nlines, const QString &s);

    private:
        void loop();

    private:
        TextRepo::SavePartStmt stmtSavePart_;
        TaskQueue<void(Worker &worker)> &taskQueue_;
        std::thread thread_;
        std::atomic_bool stopping_{ false };
        std::unique_ptr<RenderConfig> config_;
        std::unique_ptr<GlyphWidthCache> widthProvider_;
    };

    void onPartLoaded(const doc::PartLoadedEvent &e);

    RowN updatePartInfo(const PartInfo &i);

    // 更新段偏移信息
    // 多线程加载导致各个片段不是完全有序的，但总体上是有序的
    // 所以每次加载完一个片段后检查下这个片段前的片段是否都加载完成
    // 如果这个片段的前面都加载完成，或者因为这一片段的完成使得位置在其后却先加载的先片段明确了段偏移，则更新它门段偏移
    // 如果没有加载完成，则临时存下来，等后面的片段来更新
    void updateRowOff(const PartInfo &i);

    const PartInfo *findPartByRow(RowN row) const;

private:
    RenderConfig config_;
    TaskQueue<void(Worker &worker)> taskQueue_;
    std::vector<std::unique_ptr<Worker>> workers_;
    
    SigConns sigConns_;
    Signal<void(RowN)> sigRowCountUpdated_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;

private:
    mutable std::mutex mtx_;
    std::map<i64, PartInfo> byteOffToInfos_;

    // 已经确定段数偏移的
    // 键：字节偏移，值：段数偏移
    std::map<i64, RowN> rowOffAwaredParts_;

    // 未确定段数偏移的
    // 键：字节偏移，值：这一片段内的段数
    std::map<i64, RowN> rowOffUnawaredParts_;

    RowN rowCount_ = 0;
    RowN lineCount_ = 0;

    struct WaitingRow {
        WaitingRow(RowN row, std::function<void(QString)> &&cb)
            : row(row), cb(std::move(cb)) {}

        WaitingRow(WaitingRow &&b) noexcept {
            row = b.row;
            b.row = 0;
            cb = std::move(b.cb);
        }

        WaitingRow &operator=(WaitingRow &&b) noexcept {
            if (this != &b) {
                row = b.row;
                b.row = 0;
                cb = std::move(b.cb);
            }
            return *this;
        }

        RowN row = 0;
        std::function<void(QString)> cb;
    };
    std::optional<WaitingRow> waitingRow_;
};

}

