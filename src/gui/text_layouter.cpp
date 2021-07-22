#include "text_layouter.h"

#include <string_view>
#include <string>
#include <sstream>
#include <future>

#include "core/time_util.h"
#include "core/logger.h"
#include "core/readable_size_util.h"
#include "doc/document.h"
#include "text/utf16char_in_stream.h"
#include "row_walker.h"


namespace gui::detail
{

RowN TextLayouterImpl::countLines(const MemBuff &utf16data) const
{
    RowN lineCount = 0;

    std::u16string content(reinterpret_cast<const char16_t *>(utf16data.data()), utf16data.size() / 2);
    std::basic_istringstream<char16_t> ss(content);

    std::u16string line;
    while (std::getline(ss, line)) {
        UTF16CharInStream u16chars(line.data(), line.size() * 2);
        CharInStreamOverUTF16CharInStram charStream(u16chars);
        RowWalker walker(config_, width_, charStream);

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

TextLayouterImpl::TextLayouterImpl(StrandPool &pool, const TextAreaConfig &config, int width, doc::Document &document)
    : worker_(pool.allocate("LineCounter"))
    , config_(config)
    , width_(width)
    , document_(document)
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
