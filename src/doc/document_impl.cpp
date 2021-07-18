#include "document_impl.h"

#include <iostream>
#include <future>

#include "core/logger.h"
#include "core/time_util.h"
#include "core/system_util.h"
#include "core/readable_size_util.h"
#include "core/charset_converter.h"

#include "sql/asset.prepare_db.sql.h"


namespace doc::detail
{

DocumentImpl::DocumentImpl(const fs::path &path, Worker &ownerThread)
    : path_(path)
    , asyncComponents_(new AsyncComponents(path))
    , ownerThread_(ownerThread)
{
    LOGD << "DocumentImpl::DocumentImpl(...) for [" << path << "]";
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

void DocumentImpl::loadPart(AsyncComponents &comps, const LoadingPartInfo &info)
{
    const char *title = "DocumentImpl::loadPart() ";

    ElapsedTime elapsedTime;

    CharsetConverter converter;
    converter.open(info.charset, Charset::UTF_8);

    MemBuff &decodeBuff = comps.decodeBuff();
    decodeBuff.clear();

    converter.convert(comps.readBuff(), decodeBuff);

    Statement &saveDataStmt = comps.saveDataStatement();
    saveDataStmt.reset();

    saveDataStmt.bindNull(1);
    saveDataStmt.bind(2, info.off);
    saveDataStmt.bind(3, info.len);
    saveDataStmt.bind(4, decodeBuff.data(), decodeBuff.size());

    saveDataStmt.step();

    LOGD << title << "end, off[" << info.off << "], len[" << info.len << "], charset[" << info.charset
        << "], time usage[" << elapsedTime.milliSec() << " ms]";
}

static bool isDatabaseEmpty(const fs::path &dbPath)
{
    if (!fs::exists(dbPath)) {
        return true;
    }
    if (fs::file_size(dbPath) == 0) {
        return true;
    }
    return false;
}

bool DocumentImpl::prepareDatabase()
{
    // const bool isDbEmpty = isDatabaseEmpty(dbPath_);

    try {

        const fs::path dbPath = path_.generic_string() + ".notesharp.db";

        {
            // TODO 开发时为了方便每次清空旧数据文件
            std::ofstream ofs(dbPath, std::ios::binary);
        }

        db_.open(dbPath);

        db_.exec(reinterpret_cast<const char *>(Asset::Data::prepare_db__sql));

        return true;
    }
    catch (const std::exception &e) {
        LOGE << "DocumentImpl::prepareDatabase() failed because" << ": " << e.what();
        return false;
    }
    catch (...) {
        LOGE << "DocumentImpl::prepareDatabase() failed because" << "unknown exception";
        return false;
    }
}

bool DocumentImpl::prepareSaveDataStatement(Statement &stmt)
{
    try {

        stmt.open(db_, "INSERT INTO doc VALUES(?,?,?,?);");

        return true;
    }
    catch (std::exception &e) {
        LOGE << "DocumentImpl::prepareSaveDataStatement() failed because" << ": " << e.what();
        return false;
    }
    catch (...) {
        LOGE << "DocumentImpl::prepareSaveDataStatement() failed because" << "unknown exception";
        return false;
    }
}

void DocumentImpl::loadDocument(AsyncComponents &comps)
{
    static const char *title = "DocumentImpl::loadDocument() ";

    if (!prepareDatabase()) {
        return;
    }

    if (!prepareSaveDataStatement(comps.saveDataStatement())) {
        return;
    }

    ElapsedTime elapsedTime;

    LOGD << title << "start for file [" << path_ << "]";

    std::ifstream &ifs = comps.ifs();
    MemBuff &readBuff = comps.readBuff();
    CharsetDetector &charsetDetector = comps.charsetDetector();

    const uintmax_t partLen = partSize();

    uintmax_t partLenSum = 0;

    for (uintmax_t partIndex = 0; ifs; ++partIndex) {

        const uintmax_t offset = ifs.tellg();

        readBuff.reverse(partLen);
        ifs.read(reinterpret_cast<char *>(readBuff.data()), partLen);
        const uintmax_t gcount = ifs.gcount();
        readBuff.resize(gcount);

        charsetDetector.feed(readBuff.data(), gcount);
        charsetDetector.end();

        const std::string scharset = charsetDetector.charset();
        const Charset charset = CharsetUtil::strToCharset(scharset);

        moveFileStreamPosToAfterNewLine(charset, ifs, readBuff);

        LoadingPartInfo info;
        info.off = offset;
        info.len = readBuff.size();
        info.charset = charset;

        loadPart(comps, info);

        //LOGD << title << " part(" << partIndex << ") gcount [" << gcount << "], part len: [" << buff.size() << "]";

        partLenSum += readBuff.size();

        readBuff.clear();
    }

    comps.readBuff().clear();
    comps.decodeBuff().clear();

    LOGD << title << "end for file [" << path_ << "]";
    LOGD << title << "part len sum : [" << partLenSum << "](" << ReadableSizeUtil::convert(partLenSum, 2) << ")";
    LOGD << title << "time usage : " << elapsedTime.milliSec() << "ms";
}

void DocumentImpl::asyncLoadDocument()
{
    auto self(shared_from_this());
    std::async(std::launch::async, [this, self, comps = asyncComponents_] {
        loadDocument(*comps);
        ownerThread_.post([this, self, comps] {
            asyncComponents_ = comps;
            });
        });
}

DocumentImpl::~DocumentImpl()
{

}

void DocumentImpl::start()
{
    asyncLoadDocument();
}

void DocumentImpl::bind(DocumentListener &listener)
{
    listener_ = &listener;
}

void DocumentImpl::unbind()
{
    listener_ = nullptr;
}

bool DocumentImpl::loaded() const
{
    return loaded_;
}

RowN DocumentImpl::loadedRowCount() const
{
    return loadedRowCount_;
}

Charset DocumentImpl::charset() const
{
    return charset_;
}

}
