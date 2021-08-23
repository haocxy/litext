#include "text_loader.h"

#include <chrono>

#include "core/logger.h"
#include "core/time.h"
#include "core/system.h"
#include "core/thread.h"
#include "core/charset.h"
#include "core/readable.h"

#include "charset/to_utf32.h"

#include "skip_row.h"


namespace doc
{

static int decideDecoderCount()
{
    return std::max(1, SystemUtil::processorCount() / 2);
}

TextLoader::TextLoader(const fs::path &docPath)
    : loadingParts_(decideDecoderCount())
{
    reader_ = std::make_unique<Reader>(*this, docPath, readerTasks_, loadingParts_);

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

static i64 partSize() {
#ifndef NDEBUG
    return SystemUtil::pageSize() * 128;
#else
    return SystemUtil::pageSize() * 1024;
#endif
}

void TextLoader::loadAll()
{
    readerTasks_.push([](Reader &reader) {
        reader.readAll();
    });
}

static bool shouldDetectCharset(Charset current) {
    if (current == Charset::Unknown) {
        return true;
    }
    if (current == Charset::Ascii) {
        return true;
    }
    // 已经识别到有效的非ASCII编码，则不再检测编码
    return false;
}

Charset TextLoader::updateCharset(const MemBuff &data)
{
    {
        std::unique_lock<std::mutex> lock(mtx_);

        if (!shouldDetectCharset(charset_)) {
            return charset_;
        }
    }

    CharsetDetector charsetDetector;
    charsetDetector.feed(data.data(), data.size());
    charsetDetector.end();
    const char *scharset = charsetDetector.charset();
    assert(scharset);

    const Charset detectedCharset = CharsetUtil::strToCharset(scharset);

    {
        std::unique_lock<std::mutex> lock(mtx_);
        charset_ = detectedCharset;
    }

    return detectedCharset;
}

TextLoader::Reader::Reader(TextLoader &self, const fs::path &docPath, TaskQueue<void(Reader &)> &tasks, LoadingParts &loadingParts)
    : self_(self)
    , docPath_(docPath)
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
    constexpr i32 SkipRowBytesLimit = 100 * 1024 * 1024;

    static const char *title = "TextLoader::loadAll() ";

    ElapsedTime elapsedTime;

    std::ifstream ifs(docPath_, std::ios::binary);

    const i64 partLen = partSize();

    i64 partLenSum = 0;

    for (i64 partIndex = 0; (!stopping_) && ifs; ++partIndex) {

        const i64 offset = ifs.tellg();

        MemBuff readBuff;
        readBuff.reverse(partLen);
        ifs.read(reinterpret_cast<char *>(readBuff.data()), partLen);
        const i64 gcount = ifs.gcount();
        readBuff.resize(gcount);

        const Charset charset = self_.updateCharset(readBuff);

        if (!skipRow(charset, ifs, readBuff, SkipRowBytesLimit)) {
            self_.sigFatalError_(DocError::RowTooBig);
            LOGE << title << "row too big in doc [" << docPath_ << "]";
            return;
        }

        LoadingPart p;
        p.off = offset;
        p.filesize = fs::file_size(docPath_);
        p.charset = charset;
        p.isLast = ifs.eof();
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

    std::u32string content = charset::toUTF32(p.charset, p.data.data(), p.data.size());

    LOGD << title << "end, off[" << p.off << "], len[" << p.data.size() << "], charset[" << p.charset << "], time usage[" << elapsedTime.milliSec() << " ms]";

    DecodedPart e;
    e.setFileSize(p.filesize);
    e.setByteOffset(p.off);
    e.setPartSize(p.data.size());
    e.setIsLast(p.isLast);
    e.setContent(std::move(content));

    self_.sigPartDecoded_(e);
}

}
