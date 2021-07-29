#include "text_loader.h"

#include <chrono>

#include <QTextCodec>

#include "core/logger.h"
#include "core/time.h"
#include "core/system.h"
#include "core/thread.h"
#include "core/charset.h"
#include "core/readable.h"

#include "skip_row.h"


namespace doc
{

static int decideDecoderCount()
{
    return std::max(1, SystemUtil::processorCount() / 2);
}

TextLoader::TextLoader(const fs::path &docPath)
    : loadingParts_(decideDecoderCount())
    , textRepo_(docPath.generic_string() + ".notesharpdb")
{
    reader_ = std::make_unique<Reader>(docPath, readerTasks_, loadingParts_);

    const int decoderCount = decideDecoderCount();

    for (int i = 0; i < decoderCount; ++i) {
        decoders_.push_back(std::make_unique<Decoder>(*this, loadingParts_));
    }
}

TextLoader::~TextLoader()
{
    readerTasks_.stop();
    loadingParts_.stop();
    reader_ = nullptr;
    decoders_.clear();
}

static uintmax_t partSize() {
    return SystemUtil::pageSize() * 1024;
}

void TextLoader::loadAll()
{
    readerTasks_.push([](Reader &reader) {
        reader.readAll();
    });
}

TextLoader::Reader::Reader(const fs::path &docPath, TaskQueue<void(Reader &)> &tasks, LoadingParts &loadingParts)
    : docPath_(docPath)
    , tasks_(tasks)
    , loadingParts_(loadingParts)
    , th_([this] { loop(); })
{
    
}

TextLoader::Reader::~Reader()
{
    stopping_ = true;
    th_.join();
}

void TextLoader::Reader::readAll()
{
    static const char *title = "TextLoader::loadAll() ";

    ElapsedTime elapsedTime;

    std::ifstream ifs(docPath_, std::ios::binary);

    
    CharsetDetector charsetDetector;

    const uintmax_t partLen = partSize();

    uintmax_t partLenSum = 0;

    for (uintmax_t partIndex = 0; (!stopping_) && ifs; ++partIndex) {

        const uintmax_t offset = ifs.tellg();

        MemBuff readBuff;
        readBuff.reverse(partLen);
        ifs.read(reinterpret_cast<char *>(readBuff.data()), partLen);
        const uintmax_t gcount = ifs.gcount();
        readBuff.resize(gcount);

        charsetDetector.feed(readBuff.data(), gcount);
        charsetDetector.end();

        const std::string scharset = charsetDetector.charset();
        const Charset charset = CharsetUtil::strToCharset(scharset);

        skipRow(charset, ifs, readBuff);

        LoadingPart p;
        p.off = offset;
        p.filesize = fs::file_size(docPath_);
        p.charset = charset;
        p.data = std::move(readBuff);

        loadingParts_.push(std::move(p));

        partLenSum += readBuff.size();

        readBuff.clear();
    }

    LOGD << title << "part len sum : [" << partLenSum << "](" << ReadableSizeUtil::convert(partLenSum, 2) << ")";
    LOGD << title << "time usage : " << elapsedTime.milliSec() << "ms";
}

void TextLoader::Reader::loop()
{
    ThreadUtil::setNameForCurrentThread("TextLoader-Reader");
    while (!stopping_) {
        auto task = tasks_.pop();
        if (task) {
            (*task)(*this);
        }
    }
}

TextLoader::Decoder::Decoder(TextLoader &self, LoadingParts &loadingParts)
    : self_(self)
    , loadingParts_(loadingParts)
    , stmtSavePart_(self.textRepo_.stmtSavePart())
    , th_([this]() { loop(); })
{
}

TextLoader::Decoder::~Decoder()
{
    stopping_ = true;
    th_.join();
}

void TextLoader::Decoder::loop()
{
    while (!stopping_) {
        std::optional<LoadingPart> part = loadingParts_.pop();
        if (part) {
            decodePart(std::move(*part));
        }
    }
}

void TextLoader::Decoder::decodePart(LoadingPart &&p)
{
    const char *title = "TextLoader::loadPart() ";

    ElapsedTime elapsedTime;

    QTextCodec *codec = QTextCodec::codecForName(CharsetUtil::charsetToStr(p.charset));
    if (!codec) {
        std::ostringstream ss;
        ss << "cannot get codec by charset name [" << CharsetUtil::charsetToStr(p.charset) << "]";
        throw std::logic_error(ss.str());
    }

    std::unique_ptr<QTextDecoder> decoder(codec->makeDecoder());

    QString content = decoder->toUnicode(reinterpret_cast<const char *>(p.data.data()), p.data.size());

    const i64 id = stmtSavePart_(p.off, content.constData(), static_cast<i64>(content.size()) * 2);

    LOGD << title << "end, off[" << p.off << "], len[" << p.data.size() << "], charset[" << p.charset << "], time usage[" << elapsedTime.milliSec() << " ms]";

    PartLoadedEvent e;
    e.setPartId(id);
    e.setFileSize(p.filesize);
    e.setPartOffset(p.off);
    e.setPartSize(p.data.size());
    e.setContent(std::move(content));

    self_.sigPartLoaded_(e);
}

}
