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


LineManagerImpl::LineManagerImpl(const TextAreaConfig &config, int width, doc::Document &document)
    : config_(config)
    , document_(document)
{
    NewRowWalker::Config walkerConfig(config, width);

    const int workerCount = std::max(4, SystemUtil::processorCount() / 2);

    for (int i = 0; i < workerCount; ++i) {
        workers_.push_back(std::make_unique<Worker>(config_.fontIndex(), taskQueue_, walkerConfig));
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
    const NewRowWalker::Config &config)
    : taskQueue_(taskQueue)
    , thread_([this] { loop(); })
    , widthProvider_(fontIndex, 22)
    , config_(config)
{

}

LineManagerImpl::Worker::~Worker() {
    stopping_ = true;
    thread_.join();
}

LineManagerImpl::PartInfo LineManagerImpl::Worker::countLines(const MemBuff &utf16data)
{
    RowN rowCount = 0;
    RowN lineCount = 0;

    std::u16string content(reinterpret_cast<const char16_t *>(utf16data.data()), utf16data.size() / 2);

    QString qstrData = QString::fromStdU16String(content);
    QTextStream qtextStream(&qstrData);

    while (true) {
        QString line = qtextStream.readLine();
        if (line.isNull()) {
            break;
        }

        ++rowCount;
        UTF16CharInStream u16chars(line.data(), line.size() * 2);
        CharInStreamOverUTF16CharInStram charStream(u16chars);
        //RowWalker walker(config_, width_, charStream);
        NewRowWalker walker(widthProvider_, charStream, config_);

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
