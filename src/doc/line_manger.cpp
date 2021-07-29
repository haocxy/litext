#include "line_manger.h"

#include <string_view>
#include <string>
#include <sstream>
#include <future>

#include <QTextStream>

#include "core/thread_util.h"
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

LineManager::LineManager(TextLoader &loader)
    : loader_(loader)
    , taskQueue_(decideWorkerCount())
{
    const int workerCount = decideWorkerCount();

    for (int i = 0; i < workerCount; ++i) {
        workers_.push_back(std::make_unique<Worker>(taskQueue_));
    }

    sigConns_ += loader_.sigPartLoaded().connect([this](const doc::PartLoadedEvent &e) {
        onPartLoaded(e);
    });
}

LineManager::~LineManager()
{
    taskQueue_.stop();

    workers_.clear();
}

void LineManager::updateConfig(const RenderConfig &config)
{
    config_ = config;

    for (const auto &worker : workers_) {
        worker->updateConfig(config);
    }
}

void LineManager::onPartLoaded(const doc::PartLoadedEvent &e)
{
    taskQueue_.push([this, e](Worker &worker) {
        const int64_t partId = e.partId();
        ElapsedTime elapse;
        const PartInfo partInfo = worker.countLines(e.utf16content());
        LOGD << "LineManager part[" << partId << "], linecount[" << partInfo.lineCount << "], time usage[" << elapse.milliSec() << "]";
        const RowN totalRowCount = updatePartInfo(partId, partInfo);
        sigRowCountUpdated_(totalRowCount);
        sigPartLoaded_(e);
    });
}

RowN LineManager::updatePartInfo(int64_t id, const PartInfo &newInfo)
{
    std::unique_lock<std::mutex> lock(mtxPartInfos_);
    PartInfo &info = partIdToInfos_[id];
    rowCount_ -= info.rowCount;
    lineCount_ -= info.lineCount;
    info = newInfo;
    rowCount_ += info.rowCount;
    lineCount_ += info.lineCount;
    return rowCount_;
}

LineManager::Worker::Worker(TaskQueue<void(Worker &worker)> &taskQueue)
    : taskQueue_(taskQueue)
    , thread_([this] { loop(); })
{

}

LineManager::Worker::~Worker() {
    stopping_ = true;
    thread_.join();
}

void LineManager::Worker::updateConfig(const RenderConfig &config)
{
    config_ = std::make_unique<RenderConfig>(config);
    widthProvider_ = std::make_unique<GlyphWidthCache>(config_->font(), 22);
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
        UTF16CharInStream u16chars(line.data(), line.size() * 2);
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
