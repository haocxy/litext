#include "line_manger.h"

#include <string_view>
#include <string>
#include <sstream>
#include <future>

#include <QTextStream>

#include "core/thread_util.h"
#include "core/system_util.h"
#include "core/time_util.h"
#include "core/logger.h"
#include "core/readable_size_util.h"
#include "doc/document.h"
#include "text/utf16char_in_stream.h"
#include "row_walker.h"


namespace gui::detail
{

// 工作线程数量
// 使用一半的CPU，确保用户体验
static int decideWorkerCount()
{
    return std::max(1, SystemUtil::processorCount() / 2);
}

LineManagerImpl::LineManagerImpl(const TextAreaConfig &config, int width, doc::Document &document)
    : taskQueue_(decideWorkerCount())
    , config_(config.horizontalTextLayout())
    , document_(document)
{
    const int workerCount = decideWorkerCount();

    for (int i = 0; i < workerCount; ++i) {
        workers_.push_back(std::make_unique<Worker>(config.fontIndex(), taskQueue_, config_, width));
    }

    sigConns_ += document_.sigPartLoaded().connect([this](const doc::PartLoadedEvent &e) {
        onPartLoaded(e);
    });
}

LineManagerImpl::~LineManagerImpl()
{
    taskQueue_.stop();
}

void LineManagerImpl::onPartLoaded(const doc::PartLoadedEvent &e)
{
    auto self(shared_from_this());
    taskQueue_.push([this, self, e](Worker &worker) {
        const int64_t partId = e.partId();
        ElapsedTime elapse;
        const PartInfo partInfo = worker.countLines(e.utf16content());
        LOGD << "LineManager part[" << partId << "], linecount[" << partInfo.lineCount << "], time usage[" << elapse.milliSec() << "]";
        const RowN totalRowCount = updatePartInfo(partId, partInfo);
        sigRowCountUpdated_(totalRowCount);
    });
}

RowN LineManagerImpl::updatePartInfo(int64_t id, const PartInfo &newInfo)
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

LineManagerImpl::Worker::Worker(
    const font::FontIndex &fontIndex,
    BlockQueue<std::function<void(Worker &worker)>> &taskQueue,
    const HLayoutConfig &config,
    int widthLimit)
    : taskQueue_(taskQueue)
    , thread_([this] { loop(); })
    , widthProvider_(fontIndex, 22)
    , config_(config)
    , widthLimit_(widthLimit)
{

}

LineManagerImpl::Worker::~Worker() {
    stopping_ = true;
    thread_.join();
}

LineManagerImpl::PartInfo LineManagerImpl::Worker::countLines(const QString &content)
{
    RowN rowCount = 0;
    RowN lineCount = 0;

    QTextStream qtextStream(const_cast<QString *>(&content), QIODevice::ReadOnly);

    while (true) {
        QString line = qtextStream.readLine();
        if (line.isNull()) {
            break;
        }

        ++rowCount;
        UTF16CharInStream u16chars(line.data(), line.size() * 2);
        CharInStreamOverUTF16CharInStram charStream(u16chars);
        NewRowWalker walker(widthProvider_, charStream, config_, widthLimit_);

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


void LineManagerImpl::Worker::loop() {
    ThreadUtil::setNameForCurrentThread("LineManager-Worker");
    while (!stopping_) {
        auto f = taskQueue_.pop();
        if (f) {
            (*f)(*this);
        }
    }
}

}
