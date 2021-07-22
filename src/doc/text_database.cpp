#include "text_database.h"

#include <chrono>

#include "core/logger.h"
#include "core/time_util.h"
#include "core/system_util.h"
#include "core/charset_detector.h"
#include "core/charset_converter.h"
#include "core/readable_size_util.h"

#include "sql/asset.prepare_db.sql.h"


namespace doc::detail
{

TextDatabaseImpl::TextDatabaseImpl(const fs::path &docPath)
    : docPath_(docPath)
    , dbPath_(docPath.generic_string() + ".notesharpdb")
    , ifs_(docPath, std::ios::binary)
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

bool TextDatabaseImpl::prepareDatabase()
{
    if (shouldClearDbFile(dbPath_)) {
        clearFile(dbPath_);
        LOGD << "TextDatabaseImpl::prepareDatabase() db file [" << dbPath_ << "] cleared";
    }

    try {
        db_.open(dbPath_);

        db_.exec(reinterpret_cast<const char *>(Asset::Data::prepare_db__sql));

        return true;
    }
    catch (const std::exception &e) {
        LOGE << "TextDatabaseImpl::prepareDatabase() failed because" << ": " << e.what();
        return false;
    }
    catch (...) {
        LOGE << "TextDatabaseImpl::prepareDatabase() failed because" << "unknown exception";
        return false;
    }
}

bool TextDatabaseImpl::prepareSaveDataStatement()
{
    try {

        saveDataStmt_.open(db_, "INSERT INTO doc VALUES(?,?,?,?);");

        return true;
    }
    catch (std::exception &e) {
        LOGE << "TextDatabaseImpl::prepareSaveDataStatement() failed because" << ": " << e.what();
        return false;
    }
    catch (...) {
        LOGE << "TextDatabaseImpl::prepareSaveDataStatement() failed because" << "unknown exception";
        return false;
    }
}

static uintmax_t partSize() {
    return SystemUtil::pageSize() * 1024;
}

static void skipRowEndForAscii(std::ifstream &ifs, std::vector<unsigned char> &skipBuff) {
    constexpr char eof = std::ifstream::traits_type::eof();
    char ch = 0;
    while ((ch = ifs.get()) != eof) {
        skipBuff.push_back(ch);
        if (ch == '\r') {
            char next = 0;
            if ((next = ifs.get()) != eof) {
                if (next == '\n') {
                    skipBuff.push_back(next);
                    return;
                } else {
                    ifs.unget();
                    continue;
                }
            } else {
                return;
            }
        } else if (ch == '\n') {
            return;
        } else {
            continue;
        }
    }
}

static void skipRowEndForUnknown(std::ifstream &ifs, std::vector<unsigned char> &skipBuff) {
    skipRowEndForAscii(ifs, skipBuff);
}

// 把文件流ifs定位到换行字节组之后的下一个字节
static void skipRowEnd(std::ifstream &ifs, Charset charset, std::vector<unsigned char> &skipBuff) {
    switch (charset) {
    case Charset::Ascii:
    case Charset::UTF_8:
    case Charset::GB18030:
        skipRowEndForAscii(ifs, skipBuff);
        return;
    default:
        skipRowEndForUnknown(ifs, skipBuff);
        return;
    }
}

static void moveFileStreamPosToAfterNewLine(Charset charset, std::ifstream &ifs, MemBuff &partBuff)
{
    std::vector<unsigned char> skipBuff;
    skipRowEnd(ifs, charset, skipBuff);
    if (!skipBuff.empty()) {
        partBuff.append(skipBuff.data(), skipBuff.size());
    }
}

void TextDatabaseImpl::loadPart(const MemBuff &readBuff, const LoadingPartInfo &info)
{
    const char *title = "TextDatabaseImpl::loadPart() ";

    ElapsedTime elapsedTime;

    CharsetConverter converter;
    converter.open(info.charset, Charset::UTF_16);

    MemBuff decodeBuff = converter.convert(readBuff);

    saveDataStmt_.reset();
    saveDataStmt_.arg().arg(info.off).arg(info.len).arg(decodeBuff);
    saveDataStmt_.step();

    LOGD << title << "end, off[" << info.off << "], len[" << info.len << "], charset[" << info.charset
        << "], time usage[" << elapsedTime.milliSec() << " ms]";

    PartLoadedEvent e;
    e.setPartId(saveDataStmt_.lastInsertRowId());
    e.setFileSize(fs::file_size(docPath_));
    e.setPartOffset(info.off);
    e.setPartSize(info.len);
    e.utf16content() = std::move(decodeBuff);

    sigPartLoaded_(e);
}

void TextDatabaseImpl::loadAll()
{
    static const char *title = "TextDatabaseImpl::loadAll() ";

    if (!prepareDatabase()) {
        // TODO
        return;
    }

    if (!prepareSaveDataStatement()) {
        // TODO
        return;
    }

    ElapsedTime elapsedTime;

    LOGD << title << "start for file [" << dbPath_ << "]";

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

        moveFileStreamPosToAfterNewLine(charset, ifs_, readBuff);

        LoadingPartInfo info;
        info.off = offset;
        info.len = readBuff.size();
        info.charset = charset;

        loadPart(readBuff, info);

        partLenSum += readBuff.size();

        readBuff.clear();
    }

    readBuff.clear();

    LOGD << title << "end for file [" << dbPath_ << "]";
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
