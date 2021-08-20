#include "dbfiles.h"

#include <sstream>
#include <iomanip>

#include "core/system.h"


namespace doc::dbfiles
{

static std::u32string makeRelativeDbPathForWindows(const fs::path & docPath) {
    std::u32string result;
    bool isFirst = true;
    for (const auto e : fs::absolute(docPath)) {
        if (isFirst) {
            std::u32string s = e.generic_u32string();
            if (s.empty() || s.back() != ':') {
                throw std::logic_error("bad path format");
            }
            s.pop_back();
            result += s;
            isFirst = false;
            continue;
        }
        if (e == "\\" || e == "/") {
            continue;
        }
        if (e.empty()) {
            continue;
        }
        result.push_back('/');
        result += e.generic_u32string();
    }
    return result;
}

static std::u32string makeRelativeDbPathForUnix(const fs::path &docPath) {
    std::u32string result;
    bool isFirst = true;
    for (const auto e : fs::absolute(docPath)) {
        if (e == "\\" || e == "/") {
            continue;
        }
        if (e.empty()) {
            continue;
        }
        if (!isFirst) {
            result.push_back('/');
        } else {
            isFirst = false;
        }
        result += e.generic_u32string();
    }
    return result;
}

static std::u32string makeRelativeDbPath(const fs::path &doc) {
#ifdef WIN32
    return makeRelativeDbPathForWindows(doc);
#else
    return makeRelativeDbPathForUnix(doc);
#endif
}

static const std::u32string DbFileExt = U".db";

// 把相对路径编码为一个单一的文件名
// 这是因为目录本身会占用大量的硬盘容量，所以不能直接把文档路径映射为数据库的路径，
// 而是应该把文档路径编码为一个单一的文件名
// 编码时，把横杠作为转义字符对斜杠和横杠进行转义
static std::u32string encodeRelativePathToFileName(const std::u32string &path) {
    std::u32string result;
    for (char32_t c : path) {
        if (c == '/' || c == '\\') {
            result.push_back('-');
            result.push_back('d');
        } else if (c == '-') {
            result.push_back('-');
            result.push_back('-');
        } else if (c >= 0x80) {
            result.push_back('-');
            result.push_back('[');
            std::ostringstream ss;
            ss << std::hex << static_cast<uint32_t>(c);
            for (char ch : ss.str()) {
                result.push_back(ch);
            }
            result.push_back(']');
        } else {
            result.push_back(c);
        }
    }
    result += DbFileExt;
    return result;
}

static std::u32string decodeFileNameToRelativePath(const std::u32string &name) {
    if (name.size() <= DbFileExt.size()) {
        return std::u32string();
    }

    std::u32string result;

    enum class State {
        Normal,
        MeetEsc,
        MeetUnicode,
    };
    State st = State::Normal;
    std::string unicodeBuff;

    for (char32_t u : name.substr(0, name.size() - DbFileExt.size())) {
        switch (st) {
        case State::Normal:
            if (u == '-') {
                st = State::MeetEsc;
            } else {
                result.push_back(u);
            }
            break;
        case State::MeetEsc:
            if (u == 'd') {
                result.push_back('/');
                st = State::Normal;
            } else if (u == '-') {
                result.push_back('-');
                st = State::Normal;
            } else if (u == '[') {
                st = State::MeetUnicode;
            } else {
                st = State::Normal;
            }
            break;
        case State::MeetUnicode:
            if (u == ']') {
                uint32_t unicode = 0;
                std::istringstream ss(unicodeBuff);
                ss >> std::hex >> unicode;
                result.push_back(static_cast<char32_t>(unicode));
                unicodeBuff.clear();
                st = State::Normal;
            } else if (('0' <= u && u <= '9') || ('a' <= u && u <= 'f') || ('A' <= u && u <= 'F')) {
                unicodeBuff.push_back(static_cast<char>(u));
            } else {
                st = State::Normal;
            }
            break;
        default:
            break;
        }
    }
    return result;
}

static bool isDbFile(const fs::path &p) {
    std::u32string s = p.generic_u32string();
    if (s.size() > DbFileExt.size()) {
        return s.substr(s.size() - DbFileExt.size(), DbFileExt.size()) == DbFileExt;
    } else {
        return false;
    }
}

static fs::path dbDir() {
    return SystemUtil::userHome() / ".notesharp";
}

fs::path docPathToDbPath(const fs::path &doc)
{
    const fs::path dir = dbDir();
    const std::u32string relativeDbPath = makeRelativeDbPath(doc);
    const std::u32string dbFileName = encodeRelativePathToFileName(relativeDbPath);
    return dir / dbFileName;
}

void removeUselessDbFiles()
{
    const fs::path dir = dbDir();

    for (const auto e : fs::directory_iterator(dir)) {
        const std::u32string name = e.path().filename().generic_u32string();
        const fs::path relativePath = decodeFileNameToRelativePath(name);
        // TODO
    }
}

}
