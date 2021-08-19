#pragma once

#include <map>
#include <set>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "core/range.h"
#include "core/signal.h"
#include "core/sigconns.h"
#include "core/thread.h"
#include "core/font_index.h"
#include "part_loaded_event.h"
#include "text_repo.h"
#include "text_loader.h"
#include "glyph_width_cache.h"
#include "render_config.h"
#include "row_walker.h"
#include "row_index.h"
#include "row_range.h"


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

    using LoadRangeResult = std::shared_ptr<std::map<RowN, RowIndex>>;

    void loadRange(const RowRange &range, std::function<void(LoadRangeResult)> &&cb);

private:

    struct PartInfo {
        PartInfo() {}

        PartInfo(RowN rowCount)
            : rowRange(Ranges::byCount(rowCount)) {}

        i64 id = 0;
        Range<i64> byteRange;
        Range<RowN> rowRange;
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

        PartInfo countRows(const std::string &s);

        i64 savePart(i64 off, i64 nrows, const std::string &s);

    private:
        void loop();

    private:
        TextRepo::SavePartStmt stmtSavePart_;
        TaskQueue<void(Worker &worker)> &taskQueue_;
        std::thread thread_;
        std::atomic_bool stopping_{ false };
        uptr<RenderConfig> config_;
        uptr<GlyphWidthCache> widthProvider_;
    };

    void onPartLoaded(const doc::PartLoadedEvent &e);

    RowN updatePartInfo(const PartInfo &info);

    // 更新段偏移信息
    // 多线程加载导致各个片段不是完全有序的，但总体上是有序的
    // 所以每次加载完一个片段后检查下这个片段前的片段是否都加载完成
    // 如果这个片段的前面都加载完成，或者因为这一片段的完成使得位置在其后却先加载的先片段明确了段偏移，则更新它门段偏移
    // 如果没有加载完成，则临时存下来，等后面的片段来更新
    void updateRowOff(const PartInfo &info);

    void onRowOffDetected(const PartInfo &info);

    void checkWaitingRows(const PartInfo &info);

    std::optional<RowIndex> queryRowIndex(RowN row) const;

private:
    RenderConfig config_;
    TaskQueue<void(Worker &worker)> taskQueue_;
    std::vector<uptr<Worker>> workers_;
    
    SigConns sigConns_;
    Signal<void(RowN)> sigRowCountUpdated_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;

private:
    mutable std::mutex mtx_;

    std::vector<PartInfo> orderedInfos_;

    // 未确定段数偏移的
    // 键：字节偏移
    std::map<i64, PartInfo> pendingInfos_;

    RowN rowCount_ = 0;

    struct WaitingRange {

        WaitingRange() {}

        WaitingRange(WaitingRange &&b) noexcept {
            move(*this, b);
        }

        WaitingRange &operator=(WaitingRange &&b) noexcept {
            move(*this, b);
            return *this;
        }

        static void move(WaitingRange &to, WaitingRange &from) {
            if (&to != &from) {
                to.rowRange = from.rowRange;
                from.rowRange = Range<RowN>();

                to.waitingRows = std::move(from.waitingRows);

                to.foundRows = std::move(from.foundRows);

                to.cb = std::move(from.cb);
            }
        }

        Range<RowN> rowRange;
        std::set<RowN> waitingRows;
        std::map<RowN, RowIndex> foundRows;
        std::function<void(LoadRangeResult)> cb;
    };
    std::optional<WaitingRange> waitingRange_;
};

}

