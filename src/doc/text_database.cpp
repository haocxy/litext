#include "text_database.h"


#include "core/logger.h"
#include "core/time_util.h"
#include "core/system_util.h"
#include "core/charset_detector.h"
#include "core/charset_converter.h"
#include "core/readable_size_util.h"

#include "sql/asset.prepare_db.sql.h"


namespace doc::detail
{

TextDatabaseImpl::TextDatabaseImpl(const fs::path &docPath, IOContextStrand::Pool &pool)
    : docPath_(docPath)
    , dbPath_(docPath.generic_string() + ".sqlite3db")
    , ifs_(docPath, std::ios::binary)
    , worker_(pool, "TextDatabase")
{

}

TextDatabaseImpl::~TextDatabaseImpl()
{
}

void TextDatabaseImpl::start()
{
    asyncLoadAll();
}

bool TextDatabaseImpl::prepareDatabase()
{
    try {

        {
            // TODO 开发时为了方便每次清空旧数据文件
            std::ofstream ofs(dbPath_, std::ios::binary);
        }

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

void TextDatabaseImpl::loadPart(const MemBuff &readBuff, MemBuff &decodeBuff, const LoadingPartInfo &info)
{
    const char *title = "TextDatabaseImpl::loadPart() ";

    ElapsedTime elapsedTime;

    CharsetConverter converter;
    converter.open(info.charset, Charset::UTF_8);

    decodeBuff.clear();

    converter.convert(readBuff, decodeBuff);

    saveDataStmt_.reset();

    saveDataStmt_.bindNull(1);
    saveDataStmt_.bind(2, info.off);
    saveDataStmt_.bind(3, info.len);
    saveDataStmt_.bind(4, decodeBuff.data(), decodeBuff.size());

    saveDataStmt_.step();

    LOGD << title << "end, off[" << info.off << "], len[" << info.len << "], charset[" << info.charset
        << "], time usage[" << elapsedTime.milliSec() << " ms]";
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
    MemBuff decodeBuff;
    CharsetDetector charsetDetector;

    const uintmax_t partLen = partSize();

    uintmax_t partLenSum = 0;

    for (uintmax_t partIndex = 0; ifs_; ++partIndex) {

        const uintmax_t offset = ifs_.tellg();

        readBuff.reverse(partLen);
        ifs_.read(reinterpret_cast<char *>(readBuff.data()), partLen);
        const uintmax_t gcount = ifs_.gcount();
        readBuff.resize(gcount);

        charsetDetector.feed(readBuff.data(), gcount);
        charsetDetector.end();

        const std::string scharset = charsetDetector.charset();
        const Charset charset = CharsetUtil::strToCharset(scharset);

        moveFileStreamPosToAfterNewLine(charset, ifs_, readBuff);

        LoadingPartInfo info;
        info.off = offset;
        info.len = readBuff.size();
        info.charset = charset;

        loadPart(readBuff, decodeBuff, info);

        //LOGD << title << " part(" << partIndex << ") gcount [" << gcount << "], part len: [" << buff.size() << "]";

        partLenSum += readBuff.size();

        readBuff.clear();
    }

    readBuff.clear();
    decodeBuff.clear();

    LOGD << title << "end for file [" << dbPath_ << "]";
    LOGD << title << "part len sum : [" << partLenSum << "](" << ReadableSizeUtil::convert(partLenSum, 2) << ")";
    LOGD << title << "time usage : " << elapsedTime.milliSec() << "ms";
}

void TextDatabaseImpl::asyncLoadAll()
{
    auto self(shared_from_this());
    worker_.post([this, self] {
        loadAll();
    });
}

}
