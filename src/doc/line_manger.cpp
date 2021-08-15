#include "line_manger.h"

#include <string_view>
#include <string>
#include <sstream>
#include <future>

#include <QTextStream>

#include "core/thread.h"
#include "core/system.h"
#include "core/time.h"
#include "core/logger.h"
#include "core/readable.h"
#include "text/utf16char_in_stream.h"
#include "document.h"
#include "row_walker.h"


namespace doc
{

// 工作线程数量
// 使用一半的CPU，确保用户体验
static int decideWorkerCount()
{
    return std::max(1, SystemUtil::processorCount() / 2);
}

LineManager::LineManager(TextRepo &textRepo, TextLoader &loader)
    : taskQueue_(decideWorkerCount())
{
    const int workerCount = decideWorkerCount();

    for (int i = 0; i < workerCount; ++i) {
        workers_.push_back(std::make_unique<Worker>(textRepo, taskQueue_));
    }

    sigConns_ += loader.sigPartLoaded().connect([this](const doc::PartLoadedEvent &e) {
        onPartLoaded(e);
    });
}

LineManager::~LineManager()
{
    taskQueue_.stop();

    workers_.clear();
}

void LineManager::init(const RenderConfig &config)
{
    config_ = config;

    for (const auto &worker : workers_) {
        worker->init(config);
    }
}

void LineManager::loadRange(const RowRange &range, std::function<void(LoadRangeResult)> &&cb)
{
    std::unique_lock<std::mutex> lock(mtx_);

    const RowN right = range.right();

    RowN row = range.off();

    std::map<RowN, RowIndex> foundRows;

    if (!orderedInfos_.empty()) {

        // 找到最后一个可作为结果的段落偏移
        const RowN lastUsable = std::min(right, orderedInfos_.back().rowRange.right());

        // 把可用部分取出来
        for (; row <= lastUsable; ++row) {
            foundRows[row] = *queryRowIndex(row);
        }
    }

    // 如果没有未排序的部分，则直接回调并返回
    if (row > right) {
        using Map = std::map<RowN, RowIndex>;
        LoadRangeResult result = std::make_shared<Map>(std::move(foundRows));
        cb(result);
        return;
    }

    // 记录等待加载的信息
    WaitingRange waiting;
    waiting.rowRange = Ranges::byLeftAndRight(row, right);

    for (; row <= right; ++row) {
        waiting.waitingRows.insert(row);
    }

    waiting.foundRows = std::move(foundRows);
    waiting.cb = std::move(cb);

    waitingRange_ = std::move(waiting);
}

void LineManager::onPartLoaded(const doc::PartLoadedEvent &e)
{
    taskQueue_.push([this, e](Worker &worker) {
        ElapsedTime elapse;
        const QString &s = e.utf16content();
        PartInfo info = worker.countLines(s);
        info.id = worker.savePart(e.byteOffset(), info.rowRange.count(), info.lineCount, s);
        info.byteRange = Ranges::byOffAndLen(e.byteOffset(), e.partSize());
        LOGD << "LineManager part[" << info.id << "], linecount[" << info.lineCount << "], time usage[" << elapse.milliSec() << "]";
        const RowN totalRowCount = updatePartInfo(info);
        sigRowCountUpdated_(totalRowCount);
        sigPartLoaded_(e);
    });
}

RowN LineManager::updatePartInfo(const PartInfo &info)
{
    std::unique_lock<std::mutex> lock(mtx_);

    rowCount_ += info.rowRange.count();
    lineCount_ += info.lineCount;

    updateRowOff(info);

    return rowCount_;
}

void LineManager::updateRowOff(const PartInfo &info)
{
    pendingInfos_[info.byteRange.off()] = info;

    while (!pendingInfos_.empty()) {
        const PartInfo &firstPending = pendingInfos_.begin()->second;
        if (firstPending.byteRange.off() == 0) {
            orderedInfos_.push_back(firstPending);
            orderedInfos_.back().rowRange.setOff(0);
            pendingInfos_.erase(pendingInfos_.begin());
            onRowOffDetected(orderedInfos_.back());
        } else {
            if (orderedInfos_.empty()) {
                break;
            } else {
                const PartInfo &lastOrdered = orderedInfos_.back();
                if (orderedInfos_.back().byteRange.end() == firstPending.byteRange.off()) {
                    const RowN oldLastRowEnd = lastOrdered.rowRange.end();
                    orderedInfos_.push_back(firstPending);
                    orderedInfos_.back().rowRange.setOff(oldLastRowEnd);
                    pendingInfos_.erase(pendingInfos_.begin());
                    onRowOffDetected(orderedInfos_.back());
                } else {
                    break;
                }
            }
        }
    }
}

void LineManager::checkWaitingRows(const PartInfo &info)
{
    if (!waitingRange_) {
        return;
    }

    const Range<RowN> detectedRowRange = info.rowRange;

    if (!waitingRange_->rowRange.isIntersect(detectedRowRange)) {
        return;
    }

    std::set<RowN> &waitingRows = waitingRange_->waitingRows;

    for (auto it = waitingRows.begin(); it != waitingRows.end(); ) {
        const RowN row = *it;
        if (detectedRowRange.contains(row)) {
            it = waitingRows.erase(it);
            waitingRange_->foundRows[row] = *queryRowIndex(row);
        } else {
            ++it;
        }
    }

    if (waitingRows.empty()) {
        if (waitingRange_->cb) {
            waitingRange_->cb(std::make_shared<std::map<RowN, RowIndex>>(std::move(waitingRange_->foundRows)));
        }
        waitingRange_ = std::nullopt;
    }
}

void LineManager::onRowOffDetected(const PartInfo &i)
{
    LOGD << "LineManager::onRowOffDetected"
        << ", part id: [" << i.id << "]"
        << ", byte off: [" << i.byteRange.off() << "]"
        << ", row off: [" << i.rowRange.off() << "]"
        << ", row count: [" << i.rowRange.count() << "]"
        << ", row end: [" << i.rowRange.end() << "]";

    checkWaitingRows(i);
}

std::optional<RowIndex> LineManager::queryRowIndex(RowN row) const
{
    if (orderedInfos_.empty()) {
        return std::nullopt;
    }

    if (orderedInfos_.back().rowRange.right() < row) {
        return std::nullopt;
    }

    size_t leftPartIndex = 0;
    size_t rightPartIndex = orderedInfos_.size() - 1;

    while (leftPartIndex <= rightPartIndex) {
        const size_t midPartIndex = (leftPartIndex + rightPartIndex) / 2;
        const PartInfo &part = orderedInfos_[midPartIndex];

        if (row < part.rowRange.left()) {
            rightPartIndex = midPartIndex - 1;
        } else if (row > part.rowRange.right()) {
            leftPartIndex = midPartIndex + 1;
        } else {
            return RowIndex(midPartIndex, part.byteRange);
        }
    }

    throw std::logic_error("logic should never reach here");
}

LineManager::Worker::Worker(TextRepo &textRepo, TaskQueue<void(Worker &worker)> &taskQueue)
    : stmtSavePart_(textRepo)
    , taskQueue_(taskQueue)
    , thread_([this] { loop(); })
{

}

LineManager::Worker::~Worker() {
    stopping_ = true;
    thread_.join();
}

void LineManager::Worker::init(const RenderConfig &config)
{
    config_ = std::make_unique<RenderConfig>(config);
    widthProvider_ = std::make_unique<GlyphWidthCache>(config_->font(), 22);
}

void LineManager::Worker::setWidthLimit(int w)
{
    std::unique_ptr<RenderConfig> config(new RenderConfig(*config_));
    config->setWidthLimit(w);
    config_ = std::move(config);
}

LineManager::PartInfo LineManager::Worker::countLines(const QString &content)
{
    RowN rowCount = 0;
    RowN lineCount = 0;

    QTextStream qtextStream(const_cast<QString *>(&content), QIODevice::ReadOnly);

    while (!stopping_) {
        QString line = qtextStream.readLine();
        if (line.isNull()) {
            break;
        }

        ++rowCount;
        UTF16CharInStream u16chars(line.data(), static_cast<size_t>(line.size()) * 2);
        CharInStreamOverUTF16CharInStram charStream(u16chars);
        RowWalker walker(*widthProvider_, charStream, config_->hLayout(), config_->widthLimit());

        size_t lineCountInCurrentRow = 0;

        walker.forEachChar([&lineCount, &lineCountInCurrentRow](bool isEmptyRow, size_t lineIndex, const VChar &vchar) {
            if (lineIndex == lineCountInCurrentRow) {
                ++lineCountInCurrentRow;
                ++lineCount;
            }
        });
    }

    return PartInfo(rowCount, lineCount);
}

i64 LineManager::Worker::savePart(i64 off, i64 nrows, i64 nlines, const QString &s)
{
    return stmtSavePart_(off, nrows, nlines, s.constData(), static_cast<i64>(s.size()) * 2);
}

void LineManager::Worker::loop() {
    ThreadUtil::setNameForCurrentThread("LineManager-Worker");
    while (!stopping_) {
        auto f = taskQueue_.pop();
        if (f) {
            (*f)(*this);
        }
    }
}

}
