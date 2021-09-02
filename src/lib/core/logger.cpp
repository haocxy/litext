#include "logger.h"

#include <ctime>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <mutex>
#include <atomic>
#include <system_error>

#include "fs.h"

#ifdef WIN32
#include <Windows.h>
#endif

#ifndef NDEBUG
static const logger::Level DefaultLogLevel = logger::Level::All;
#else
static const logger::Level DefaultLogLevel = logger::Level::Error;
#endif

static logger::Level g_Level = logger::Level::Info;

static logger::Writer *g_writer = nullptr;


static void safeLocalTime(std::tm &tm, std::time_t sec) {
#ifdef WIN32
    localtime_s(&tm, &sec);
#else
    localtime_r(&sec, &tm);
#endif
}

static uint32_t currentThreadShortId() {
    static std::atomic<uint32_t> s_nextShortId = 1;
    thread_local uint32_t tl_curThreadShortId = s_nextShortId++;
    return tl_curThreadShortId;
}


namespace logger
{

bool shouldLog(logger::Level level) {
    return level >= g_Level;
}

Writer::Writer(const fs::path &dir, const std::string &basename)
    : base_(fs::absolute(dir / basename).lexically_normal()) {

    if (dir.empty()) {
        throw std::logic_error("dir is empty string");
    }

    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    } else {
        if (!fs::is_directory(dir)) {
            std::ostringstream ss;
            ss << "logger::Writer construct error: "
                << "[" << dir.generic_string() << "]"
                << "exists but not directory";
            throw std::logic_error(ss.str().c_str());
        }
    }

    if (fs::exists(base_)) {
        std::ostringstream ss;
        ss << "logger::Writer construct error: "
            << "base path [" << base_.generic_string() << "]"
            << "already exists";
        throw std::logic_error(ss.str().c_str());
    }

    std::memset(&lastTime_, 0, sizeof(lastTime_));
}

static std::string makeFilePath(const std::string &basePath, const std::tm &tm) {
    std::ostringstream ss;
    ss << basePath << '-';
    ss << std::setw(4) << std::setfill('0') << tm.tm_year + 1900;
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << tm.tm_mon + 1;
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << tm.tm_mday;
    ss << '_';
    ss << std::setw(2) << std::setfill('0') << tm.tm_hour;
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << tm.tm_min;
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << tm.tm_sec;
    ss << ".log";
    return ss.str();
}

void Writer::write(const void *data, size_t len) {
    if (!data || len == 0) {
        return;
    }
    const std::time_t now = std::time(nullptr);
    std::tm tm;
    std::memset(&tm, 0, sizeof(tm));
    safeLocalTime(tm, now);
    if (tm.tm_year != lastTime_.tm_year || tm.tm_mon != lastTime_.tm_mon || tm.tm_mday != lastTime_.tm_mday) {
        out_.close();
    }
    lastTime_ = tm;
    if (!out_) {
        out_.open(makeFilePath(base_.generic_string(), tm), std::ios::binary);
    }
    out_.write(reinterpret_cast<const char *>(data), len);
}

} // namespace logger

#ifndef NDEBUG
static const bool DefaultAlwaysFlush = true;
#else
static const bool DefaultAlwaysFlush = false;
#endif

static bool gAlwaysFlush = DefaultAlwaysFlush;

static bool gShouldWriteToStdout = false;

#ifdef WIN32
static bool gHasWindowsDebuger = false;
#endif

static bool shouldFlush(logger::Level level) {
    if (gAlwaysFlush) {
        return true;
    }
    switch (level) {
    case logger::Level::Warn:
    case logger::Level::Error:
        return true;
    default:
        return false;
    }
}

static std::string makeContent(logger::Level level, const LogDebugInfo &info, const std::string &content) {
    std::timespec ts;
    std::memset(&ts, 0, sizeof(ts));
    if (TIME_UTC != std::timespec_get(&ts, TIME_UTC)) {
        std::ostringstream ss;
        ss << "std::timespec_get() error: " << std::error_code(errno, std::generic_category()).message();
        throw std::runtime_error(ss.str());
    }
    std::tm tm;
    std::memset(&tm, 0, sizeof(tm));
    safeLocalTime(tm, ts.tv_sec);
    std::ostringstream buffer;
    buffer << logger::tostr(level) << '|';
    const int64_t h = tm.tm_hour;
    const int64_t m = tm.tm_min;
    const int64_t s = tm.tm_sec;
    const int64_t ms = ts.tv_nsec / 1'000'000;
    buffer << std::setw(2) << std::setfill('0') << h;
    buffer << ':';
    buffer << std::setw(2) << std::setfill('0') << m;
    buffer << ':';
    buffer << std::setw(2) << std::setfill('0') << s;
    buffer << '.';
    buffer << std::setw(3) << std::setfill('0') << ms;
    buffer << '|';
    buffer << 't' << currentThreadShortId();
    buffer << '|';
    buffer << ' ' << content;
    buffer << '\n';
    return buffer.str();
}

static std::mutex g_writeLogMutex;

static bool isConsole(const FILE *f)
{
    return true; // TODO
}

void writeLog(logger::Level level, const LogDebugInfo &info, const std::string &content) {
    std::string data = makeContent(level, info, content);

    std::lock_guard<std::mutex> lock(g_writeLogMutex);

    // 不要使用类似 std::cout / std::cerr 这类由其它库定义的全局对象记录日志
    // 因为记录日志可能发生于进入 main 函数之前，无法确保全局对象的状态

    if (g_writer) {
        g_writer->write(data.data(), data.size());
        if (shouldFlush(level)) {
            g_writer->flush();
        }
    }

    // 不管是否配置了有效的文件输出，都会检查是否设置了输出到标准输出或调试器
    if (gShouldWriteToStdout) {
        std::FILE *stdo = stdout;
        std::fwrite(data.data(), 1, data.size(), stdo);
        if (isConsole(stdo)) {
            std::fflush(stdo);
        }
    } else {
#ifdef WIN32
        if (gHasWindowsDebuger) {
            OutputDebugStringA(data.c_str());
        }
#endif
    }
}

void LogLine::printPath(const fs::path &p) {
    buffer_ << p.generic_string();
}

namespace logger::control
{

static const char *const kDefaultBaseName = "notesharp-log";

static std::string normalizeBaseName(const std::string &basename) {
    if (basename.empty()) {
        return kDefaultBaseName;
    }
    if (basename.back() == '-') {
        return basename.substr(0, basename.length() - 1);
    }
    return basename;
}

void init(const Option &opt) {

    const char *title = "logger::control::initLogger ";

    g_Level = opt.level();

    if (!opt.dir().empty()) {
        const std::string basename = normalizeBaseName(opt.basename());
        LOGI << title << "log will be write to dir: [" << opt.dir() << "] with base name [" << opt.basename() << "]";
        g_writer = new Writer(opt.dir(), basename);
    } else {
        LOGE << title << "error: [dir is empty string]. log will be write to stdout";
    }

    gAlwaysFlush = opt.isAlwaysFlush();

    gShouldWriteToStdout = opt.shouldWriteToStdout();

#ifdef WIN32
    gHasWindowsDebuger = IsDebuggerPresent();
#endif
}

static Level toLevel(const std::string &str) {
    if (str.find("a") == 0 || str == "*") {
        return Level::All;
    }
    if (str == "off" || str == "-" || str == "x") {
        return Level::Off;
    }
    if (str.find("d") == 0 || str.find("D") == 0) {
        return Level::Debug;
    }
    if (str.find("i") == 0 || str.find("I") == 0) {
        return Level::Info;
    }
    if (str.find("e") == 0 || str.find("E") == 0) {
        return Level::Error;
    }
    return DefaultLogLevel;
}

void Option::setLevel(const std::string &str) {
    level_ = toLevel(str);
}

} // namespace logger::control
