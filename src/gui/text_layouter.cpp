#include "text_layouter.h"

#include <string_view>
#include <string>
#include <sstream>
#include <future>

#include <QTextStream>

#include "core/time_util.h"
#include "core/logger.h"
#include "core/readable_size_util.h"
#include "doc/document.h"
#include "text/utf16char_in_stream.h"
#include "row_walker.h"


namespace gui::detail
{

RowN TextLayouterImpl::countLines(const MemBuff &utf16data)
{
    NewRowWalker::Config rowWalkerConfig(config_, width_);

    RowN lineCount = 0;

    std::u16string content(reinterpret_cast<const char16_t *>(utf16data.data()), utf16data.size() / 2);
    std::basic_istringstream<char16_t> ss(content);

    QString qstrData = QString::fromStdU16String(content);
    QTextStream qtextStream(&qstrData);

    try {

        while (true) {
            QString line = qtextStream.readLine();
            if (line.isNull()) {
                break;
            }
            UTF16CharInStream u16chars(line.data(), line.size() * 2);
            CharInStreamOverUTF16CharInStram charStream(u16chars);
            //RowWalker walker(config_, width_, charStream);
            NewRowWalker walker(widthProvider_, charStream, rowWalkerConfig);

            size_t lineCountInCurrentRow = 0;

            walker.forEachChar([&lineCount, &lineCountInCurrentRow](bool isEmptyRow, size_t lineIndex, const VChar &vchar) {
                if (lineIndex == lineCountInCurrentRow) {
                    ++lineCountInCurrentRow;
                    ++lineCount;
                }
            });
        }

    }
    catch (const std::exception &e) {
        LOGE << "TextLayouterImpl::countLines() exception: [" << e.what() << "]";
        throw;
    }

    return lineCount;
}

TextLayouterImpl::TextLayouterImpl(const TextAreaConfig &config, int width, doc::Document &document)
    : config_(config)
    , width_(width)
    , document_(document)
    , widthProvider_(config.fontIndex(), 22)
{
    sigConns_ += document_.sigPartLoaded().connect([this](const doc::PartLoadedEvent &e) {
        onPartLoaded(e);
    });
}

void TextLayouterImpl::onPartLoaded(const doc::PartLoadedEvent &e)
{
    auto self(shared_from_this());
    worker_.post([this, self, e] {
        LOGD << "TextLayouter::onPartLoaded() start for part id [" << e.partId() << "]";
        ElapsedTime etime;
        const int64_t partId = e.partId();
        const RowN lineCount = countLines(e.utf16content());
        {
            std::unique_lock<std::mutex> lock(mtxPartToRowCount_);
            partToLineCount_[partId] = lineCount;
        }
        LOGD << "TextLayouter::onPartLoaded() end for part id [" << partId << "] count [" << lineCount << "], time usage: [" << etime.milliSec() << " ms]";
    });
}

}
