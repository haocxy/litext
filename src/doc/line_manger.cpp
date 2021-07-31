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

void LineManager::loadRow(RowN row, std::function<void(QString s)> &&cb)
{
    std::unique_lock<std::mutex> lock(mtx_);

    const PartInfo *part = findPartByRow(row);

    if (part) {

    } else {
        waitingRow_ = WaitingRow(row, std::move(cb));
    }
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

    PartInfo &info = byteOffToInfos_[i.byteOffset];
    rowCount_ -= info.rowCount;
    lineCount_ -= info.lineCount;
    info = i;
    rowCount_ += info.rowCount;
    lineCount_ += info.lineCount;

    updateRowOff(info);

    return rowCount_;
}

void LineManager::updateRowOff(PartInfo &i)
{
    LOGD << "updateRowOff,id[" << i.id << "],byteoff[" << i.byteOffset << "],nbytes[" << i.nbytes << "]";
    // 如果当前片段是第一个片段，则直接设置第一个片段的段落偏移为0，并检查是否有未设置段落偏移的片段
    if (i.byteOffset == 0) {
        setRowOff(i, 0);
        tryMergeRowOffUnawaredParts();
    } else {
        if (rowOffAwaredParts_.empty()) {
            // 如果还没有明确段落偏移的片段，则这个片段也无法明确
            rowOffUnawaredParts_.insert(i.byteOffset);
        } else {
            const PartInfo &lastAwared = byteOffToInfos_[*(rowOffAwaredParts_.rbegin())];
            // 如果最后一个已经明确段落偏移的片段和这个片段是连续的
            if (lastAwared.byteOffset + lastAwared.nbytes == i.byteOffset) {
                setRowOff(i, lastAwared.rowOffset + lastAwared.rowCount);
                tryMergeRowOffUnawaredParts();
            } else {
                rowOffUnawaredParts_.insert(i.byteOffset);
            }
        }
    }
}

void LineManager::tryMergeRowOffUnawaredParts()
{
    if (!rowOffUnawaredParts_.empty()) {
        updateRowOff(byteOffToInfos_[*(rowOffUnawaredParts_.begin())]);
    }
}

void LineManager::setRowOff(PartInfo &i, RowN rowOff)
{
    i.rowOffset = rowOff;

    LOGD << "======> LineManager::setRowOff, part id [" << i.id << "], byteoff[" << i.byteOffset << "], row off [" << rowOff << "]"
        << ", row count: [" << i.rowCount << "], row end: [" << (i.rowOffset + i.rowCount) << "]";

    rowOffAwaredParts_.insert(i.byteOffset);
    rowOffUnawaredParts_.erase(i.byteOffset);

    // TODO 明确段落偏移时触发的逻辑
}

const LineManager::PartInfo *LineManager::findPartByRow(RowN row) const
{
    RowN partRowOffset = 0;

    for (const auto &pair : byteOffToInfos_) {
        if (row >= partRowOffset) {
            return &(pair.second);
        }
        partRowOffset += pair.second.rowCount;
    }

    return nullptr;
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
