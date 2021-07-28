#include "text_database.h"

#include <chrono>

#include <QTextCodec>

#include "core/logger.h"
#include "core/time_util.h"
#include "core/system_util.h"
#include "core/charset_detector.h"
#include "core/charset_converter.h"
#include "core/readable_size_util.h"

#include "skip_row.h"


namespace doc::detail
{

TextDatabaseImpl::TextDatabaseImpl(const fs::path &docPath)
    : docPath_(docPath)
    , ifs_(docPath, std::ios::binary)
    , textRepo_(docPath.generic_string() + ".notesharpdb")
{

}

TextDatabaseImpl::~TextDatabaseImpl()
{

}

void TextDatabaseImpl::start()
{
    asyncLoadAll();
}

void TextDatabaseImpl::stop()
{
}

static void clearFile(const fs::path &file)
{
    std::ofstream ofs(file, std::ios::binary);
}

static bool shouldClearDbFile(const fs::path &dbFile)
{
    return true;
}

static uintmax_t partSize() {
    return SystemUtil::pageSize() * 1024;
}

void TextDatabaseImpl::loadPart(const MemBuff &readBuff, const LoadingPartInfo &info)
{
    const char *title = "TextDatabaseImpl::loadPart() ";

    ElapsedTime elapsedTime;

    QTextCodec *codec = QTextCodec::codecForName(CharsetUtil::charsetToStr(info.charset));
    if (!codec) {
        std::ostringstream ss;
        ss << "cannot get codec by charset name [" << CharsetUtil::charsetToStr(info.charset) << "]";
        throw std::logic_error(ss.str());
    }

    std::unique_ptr<QTextDecoder> decoder(codec->makeDecoder());

    QString content = decoder->toUnicode(reinterpret_cast<const char *>(readBuff.data()), readBuff.size());

    const i64 id = textRepo_.savePart(info.off, content.constData(), static_cast<i64>(content.size()) * 2);

    LOGD << title << "end, off[" << info.off << "], len[" << readBuff.size() << "], charset[" << info.charset << "], time usage[" << elapsedTime.milliSec() << " ms]";

    PartLoadedEvent e;
    e.setPartId(id);
    e.setFileSize(fs::file_size(docPath_));
    e.setPartOffset(info.off);
    e.setPartSize(readBuff.size());
    e.setContent(std::move(content));

    sigPartLoaded_(e);
}

void TextDatabaseImpl::loadAll()
{
    static const char *title = "TextDatabaseImpl::loadAll() ";

    ElapsedTime elapsedTime;

    MemBuff readBuff;
    CharsetDetector charsetDetector;

    const uintmax_t partLen = partSize();

    uintmax_t partLenSum = 0;

    for (uintmax_t partIndex = 0; (!worker_.isStopping()) && ifs_; ++partIndex) {

        const uintmax_t offset = ifs_.tellg();

        readBuff.reverse(partLen);
        ifs_.read(reinterpret_cast<char *>(readBuff.data()), partLen);
        const uintmax_t gcount = ifs_.gcount();
        readBuff.resize(gcount);

        charsetDetector.feed(readBuff.data(), gcount);
        charsetDetector.end();

        const std::string scharset = charsetDetector.charset();
        const Charset charset = CharsetUtil::strToCharset(scharset);

        sigCharsetDetected_(charset);

        skipRow(charset, ifs_, readBuff);

        LoadingPartInfo info;
        info.off = offset;
        info.charset = charset;

        loadPart(readBuff, info);

        partLenSum += readBuff.size();

        readBuff.clear();
    }

    readBuff.clear();

    LOGD << title << "part len sum : [" << partLenSum << "](" << ReadableSizeUtil::convert(partLenSum, 2) << ")";
    LOGD << title << "time usage : " << elapsedTime.milliSec() << "ms";

    sigAllLoaded_();
}

void TextDatabaseImpl::asyncLoadAll()
{
    auto self(shared_from_this());
    worker_.post([this, self] {
        loadAll();
    });
}

}
