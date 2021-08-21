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
#include "core/idgen.h"
#include "part_loaded_event.h"
#include "text_repo.h"
#include "text_loader.h"
#include "row_index.h"
#include "doc_part.h"


namespace doc
{

class LineManager {
public:
    LineManager(TextRepo &textRepo, TextLoader &loader);

    ~LineManager();

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

    std::map<RowN, RowIndex> findRange(const RowRange &range);

private:

    class Worker {
    public:
        Worker(TaskQueue<void(Worker &worker)> &taskQueue);

        ~Worker();

        void stop() {
            stopping_ = true;
        }

        RowN countRows(const std::string &s);

    private:
        void loop();

    private:
        TaskQueue<void(Worker &worker)> &taskQueue_;
        std::thread thread_;
        std::atomic_bool stopping_{ false };
    };

    void onPartLoaded(const doc::PartLoadedEvent &e);

    RowN updatePartInfo(const DocPart &info);

    // 更新段偏移信息
    // 多线程加载导致各个片段不是完全有序的，但总体上是有序的
    // 所以每次加载完一个片段后检查下这个片段前的片段是否都加载完成
    // 如果这个片段的前面都加载完成，或者因为这一片段的完成使得位置在其后却先加载的先片段明确了段偏移，则更新它门段偏移
    // 如果没有加载完成，则临时存下来，等后面的片段来更新
    void updateRowOff(const DocPart &info);

    void onRowOffDetected(DocPart &info);

    std::optional<RowIndex> queryRowIndex(RowN row) const;

private:
    TextRepo::SavePartStmt stmtSavePart_;
    TaskQueue<void(Worker &worker)> taskQueue_;
    std::vector<uptr<Worker>> workers_;
    
    SigConns sigConns_;
    Signal<void(RowN)> sigRowCountUpdated_;
    Signal<void(const PartLoadedEvent &)> sigPartLoaded_;

private:
    IdGen<PartId> idGen_{ 1 };

    mutable std::mutex mtx_;

    std::vector<DocPart> orderedInfos_;

    // 未确定段数偏移的
    // 键：字节偏移
    std::map<i64, DocPart> pendingInfos_;

    RowN rowCount_ = 0;
};

}

