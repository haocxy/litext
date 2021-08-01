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

void LineManager::loadRange(RowN rowOffset, RowN rowCount, std::function<void(LoadRangeResult)> &&cb)
{
    std::unique_lock<std::mutex> lock(mtx_);

    const RowN right = rowOffset + rowCount - 1;

    RowN row = rowOffset;

    std::map<RowN, QString> orderedRows;

    if (!orderedInfos_.empty()) {

        const RowN orderedLastRow = orderedInfos_.back().rowEnd() - 1;

        // 找到最后一个可作为结果的段落偏移
        const RowN lastUsable = std::min(right, orderedLastRow);

        // 把可用部分取出来
        for (; row <= lastUsable; ++row) {
            orderedRows[row] = queryRowContent(row);
        }
    }

    // 如果没有未排序的部分，则直接回调并返回
    if (row > right) {
        using Map = std::map<RowN, QString>;
        LoadRangeResult result = std::make_shared<Map>(std::move(orderedRows));
        cb(result);
        return;
    }

    // 把未排序部分的区间记录下来
    // 这样就可以在加载和排序过程中先通过区间大致判断，
    // 如果某行在区间中才进一步判断是否是需要的行
    const RowN unorderedLeft = row;

    // 虽然，在一开始请求加载某范围的时候，未加载的部分是连续的，
    // 但是，随着加载过程的推进，未加载部分可能不再连续，
    // 所以，不能仅记录边界，而是应该用 set 记录每一个段落偏移，
    // 记录段落偏移仅仅是为了提升判断速度，并不能作为充分的依据
    std::set<RowN> unorderedRows;

    // 记录未加载的部分
    for (; row <= right; ++row) {
        unorderedRows.insert(row);
    }

    // TODO
}

void LineManager::onPartLoaded(const doc::PartLoadedEvent &e)
{
    taskQueue_.push([this, e](Worker &worker) {
        ElapsedTime elapse;
        const QString s = e.utf16content();
        PartInfo i = worker.countLines(s);
        i.id = worker.savePart(e.byteOffset(), i.rowCount, i.lineCount, s);
        i.byteOffset = e.byteOffset();
        i.nbytes = e.partSize();
        LOGD << "LineManager part[" << i.id << "], linecount[" << i.lineCount << "], time usage[" << elapse.milliSec() << "]";
        const RowN totalRowCount = updatePartInfo(i);
        sigRowCountUpdated_(totalRowCount);
        sigPartLoaded_(e);
    });
}

RowN LineManager::updatePartInfo(const PartInfo &i)
{
    std::unique_lock<std::mutex> lock(mtx_);

    rowCount_ += i.rowCount;
    lineCount_ += i.lineCount;

    updateRowOff(i);

    return rowCount_;
}

void LineManager::updateRowOff(const PartInfo &info)
{
    pendingInfos_[info.byteOffset] = info;

    while (!pendingInfos_.empty()) {
        const PartInfo &firstPending = pendingInfos_.begin()->second;
        if (firstPending.byteOffset == 0) {
            orderedInfos_.push_back(firstPending);
            orderedInfos_.back().rowOffset = 0;
            pendingInfos_.erase(pendingInfos_.begin());
            onRowOffDetected(orderedInfos_.back());
        } else {
            if (orderedInfos_.empty()) {
                break;
            } else {
                const PartInfo &lastOrdered = orderedInfos_.back();
                if (orderedInfos_.back().byteEnd() == firstPending.byteOffset) {
                    const RowN oldLastRowEnd = lastOrdered.rowEnd();
                    orderedInfos_.push_back(firstPending);
                    orderedInfos_.back().rowOffset = oldLastRowEnd;
                    pendingInfos_.erase(pendingInfos_.begin());
                    onRowOffDetected(orderedInfos_.back());
                } else {
                    break;
                }
            }
        }
    }
}

void LineManager::onRowOffDetected(const PartInfo &i)
{
    LOGD << "LineManager::onRowOffDetected"
        << ", part id: [" << i.id << "]"
        << ", byte off: [" << i.byteOffset << "]"
        << ", row off: [" << i.rowOffset << "]"
        << ", row count: [" << i.rowCount << "]"
        << ", row end: [" << i.rowEnd() << "]";

    // TODO 明确段落偏移时触发的逻辑
}

std::optional<size_t> LineManager::findPartByRow(RowN row) const
{
    if (orderedInfos_.empty()) {
        return std::nullopt;
    }

    if (orderedInfos_.back().rowEnd() <= row) {
        return std::nullopt;
    }

    size_t leftPartIndex = 0;
    size_t rightPartIndex = orderedInfos_.size() - 1;

    while (leftPartIndex <= rightPartIndex) {
        const size_t midPartIndex = (leftPartIndex + rightPartIndex) / 2;
        const PartInfo &part = orderedInfos_[midPartIndex];

        if (row < part.rowOffset) {
            rightPartIndex = midPartIndex - 1;
        } else if (row >= part.rowEnd()) {
            leftPartIndex = midPartIndex + 1;
        } else {
            return midPartIndex;
        }
    }

    throw std::logic_error("logic should never reach here");
}

QString LineManager::queryRowContent(RowN row)
{
    return QString();
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
