#pragma once

#include <atomic>
#include <memory>
#include <map>
#include <set>
#include <mutex>
#include <vector>

#include "core/range.h"
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

    using LoadRangeResult = std::shared_ptr<std::map<RowN, QString>>;

    void loadRange(RowN rowOffset, RowN rowCount, std::function<void(LoadRangeResult)> &&cb);

private:

    struct PartInfo {
        PartInfo() {}
        PartInfo(RowN rowCount, RowN lineCount)
            : rowCount(rowCount), lineCount(lineCount) {}

        RowN rowEnd() const {
            return rowOffset + rowCount;
        }

        i64 id = 0;
        Range<i64> byteRange;
        RowN rowCount = 0;
        RowN lineCount = 0;
        RowN rowOffset = 0;
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

    RowN updatePartInfo(const PartInfo &info, const QString &s);

    // 更新段偏移信息
    // 多线程加载导致各个片段不是完全有序的，但总体上是有序的
    // 所以每次加载完一个片段后检查下这个片段前的片段是否都加载完成
    // 如果这个片段的前面都加载完成，或者因为这一片段的完成使得位置在其后却先加载的先片段明确了段偏移，则更新它门段偏移
    // 如果没有加载完成，则临时存下来，等后面的片段来更新
    void updateRowOff(const PartInfo &info, const QString &s);

    void onRowOffDetected(const PartInfo &info, const QString &s);

    std::optional<size_t> findPartByRow(RowN row) const;

    QString queryRowContent(RowN row);

private:
    RenderConfig config_;
    TaskQueue<void(Worker &worker)> taskQueue_;
    std::vector<std::unique_ptr<Worker>> workers_;
    
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
    RowN lineCount_ = 0;

    struct WaitingRange {

        WaitingRange() {}

        WaitingRange(WaitingRange &&b) noexcept {
            move(*this, b);
        }

        WaitingRange &operator=(WaitingRange &b) noexcept {
            move(*this, b);
            return *this;
        }

        static void move(WaitingRange &to, WaitingRange &from) {
            if (&to != &from) {
                to.left = from.left;
                from.left = 0;

                to.right = from.right;
                from.right = 0;

                to.rows = std::move(from.rows);

                to.cb = std::move(from.cb);
            }
        }

        RowN left = 0;
        RowN right = 0;
        std::set<RowN> rows;
        std::function<void(LoadRangeResult)> cb;
    };
    std::optional<WaitingRange> waitingRange_;
};

}

