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

const LineManager::PartInfo *LineManager::findPartByRow(RowN row) const
{
    throw std::logic_error("LineManager::findPartByRow() unimplemented");
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
