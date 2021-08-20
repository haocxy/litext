#include "dbfiles.h"

#include "core/system.h"


namespace doc::dbfiles
{

static std::string makeRelativeDbPathForWindows(const fs::path & docPath) {
    std::string result;
    bool isFirst = true;
    for (const auto e : fs::absolute(docPath)) {
        if (isFirst) {
            std::string s = e.generic_string();
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
        result += e.generic_u8string();
    }
    return result;
}

static std::string makeRelativeDbPathForUnix(const fs::path &docPath) {
    std::string result;
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
        result += e.generic_u8string();
    }
    return result;
}

static std::string makeRelativeDbPath(const fs::path &doc) {
#ifdef WIN32
    return makeRelativeDbPathForWindows(doc);
#else
    return makeRelativeDbPathForUnix(doc);
#endif
}

// 把相对路径编码为一个单一的文件名
// 这是因为目录本身会占用大量的硬盘容量，所以不能直接把文档路径映射为数据库的路径，
// 而是应该把文档路径编码为一个单一的文件名
// 编码时，把横杠作为转义字符对斜杠和横杠进行转义
static std::string encodeRelativePathToFileName(const std::string &path) {
    std::string result;
    for (char c : path) {
        if (c == '/' || c == '\\') {
            result.push_back('-');
        } else if (c == '-') {
            result.push_back('-');
            result.push_back('-');
        } else {
            result.push_back(c);
        }
    }
    result += ".db";
    return result;
}

fs::path docPathToDbPath(const fs::path &doc)
{
    const fs::path dir = SystemUtil::userHome() / ".notesharp";
    const std::string relativeDbPath = makeRelativeDbPath(doc);
    const std::string dbFileName = encodeRelativePathToFileName(relativeDbPath);
    return dir / dbFileName;
}

}
