#include "text_layouter.h"

#include <string_view>
#include <string>
#include <sstream>
#include <future>

#include <QTextStream>

#include "core/thread_util.h"
#include "core/time_util.h"
#include "core/logger.h"
#include "core/readable_size_util.h"
#include "doc/document.h"
#include "text/utf16char_in_stream.h"
#include "row_walker.h"


namespace gui::detail
{


TextLayouterImpl::TextLayouterImpl(const TextAreaConfig &config, int width, doc::Document &document)
    : worker_("TextLayouter", 1)
    , config_(config)
    , width_(width)
    , document_(document)
{
    const int workCount = 4;

    NewRowWalker::Config walkerConfig(config, width);

    for (int i = 0; i < workCount; ++i) {
        workers_.push_back(std::make_unique<TextLayouterWorker>(config_.fontIndex(), taskQueue_, walkerConfig));
    }

    sigConns_ += document_.sigPartLoaded().connect([this](const doc::PartLoadedEvent &e) {
        onPartLoaded(e);
    });
}

TextLayouterImpl::~TextLayouterImpl()
{
    for (auto &worker : workers_) {
        worker->stop();
    }

    taskQueue_.stop();
}

void TextLayouterImpl::onPartLoaded(const doc::PartLoadedEvent &e)
{
    auto self(shared_from_this());
    taskQueue_.push([this, self, e](TextLayouterWorker &worker) {
        LOGD << "TextLayouter::onPartLoaded() start for part id [" << e.partId() << "]";
        ElapsedTime etime;
        const int64_t partId = e.partId();
        const RowN lineCount = worker.countLines(e.utf16content());
        {
            std::unique_lock<std::mutex> lock(mtxPartToRowCount_);
            partToLineCount_[partId] = lineCount;
        }
        LOGD << "TextLayouter::onPartLoaded() end for part id [" << partId << "] count [" << lineCount << "], time usage: [" << etime.milliSec() << " ms]";
    });
}

TextLayouterImpl::TextLayouterWorker::TextLayouterWorker(
    const font::FontIndex &fontIndex,
    BlockQueue<std::function<void(TextLayouterWorker &worker)>> &taskQueue,
    const NewRowWalker::Config &config)
    : taskQueue_(taskQueue)
    , thread_([this] { loop(); })
    , widthProvider_(fontIndex, 22)
    , config_(config)
{

}

TextLayouterImpl::TextLayouterWorker::~TextLayouterWorker() {
    stopping_ = false;
    thread_.join();
}

RowN TextLayouterImpl::TextLayouterWorker::countLines(const MemBuff &utf16data)
{
    RowN lineCount = 0;

    std::u16string content(reinterpret_cast<const char16_t *>(utf16data.data()), utf16data.size() / 2);

    QString qstrData = QString::fromStdU16String(content);
    QTextStream qtextStream(&qstrData);

    while (true) {
        QString line = qtextStream.readLine();
        if (line.isNull()) {
            break;
        }
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

    return lineCount;
}


void TextLayouterImpl::TextLayouterWorker::loop() {
    ThreadUtil::setNameForCurrentThread("TextLayouterWorker");
    while (!stopping_) {
        auto f = taskQueue_.pop();
        if (f) {
            (*f)(*this);
        }
    }
}

}
