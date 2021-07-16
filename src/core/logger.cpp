#include "logger.h"

#include <ctime>
#include <cstdio>
#include <sstream>
#include <mutex>
#include <thread>

#include "fs.h"

#include "logger-control.h"
#include "logger-writer.h"


// before logger initalized, all log should be recorded
static logger::Level g_Level = logger::Level::All;

static logger::Writer *g_writer = nullptr;


namespace logger
{

bool shouldLog(logger::Level level) {
    return level >= g_Level;
}

} // namespace logger

static bool shouldFlush(logger::Level level) {
#ifndef NDEBUG
    return true;
#endif
    switch (level) {
    case logger::Level::Warn:
    case logger::Level::Error:
        return true;
    default:
        return false;
    }
}

static void safeLocalTime(std::tm &tm, std::time_t sec) {
#ifdef WIN32
    localtime_s(&tm, &sec);
#else
    localtime_r(&sec, &tm);
#endif
}

static std::string makeContent(logger::Level level, const LogDebugInfo &info, const std::string &content) {
    std::timespec ts;
    std::memset(&ts, 0, sizeof(ts));
    if (TIME_UTC != std::timespec_get(&ts, TIME_UTC)) {
        std::ostringstream ss;
        ss << "std::timespec_get() error: " << std::strerror(errno);
        throw std::runtime_error(ss.str());
    }
    std::tm tm;
    std::memset(&tm, 0, sizeof(tm));
    safeLocalTime(tm, ts.tv_sec);
    //const ptime now = posix_time::microsec_clock::local_time();
    //const auto t = now.time_of_day();
    std::ostringstream buffer;
    buffer << logger::tostr(level) << '|';
    const int64_t h = tm.tm_hour; //static_cast<int64_t>(t.hours());
    const int64_t m = tm.tm_min; //static_cast<int64_t>(t.minutes());
    const int64_t s = tm.tm_sec; //static_cast<int64_t>(t.seconds());
    const int64_t ms = ts.tv_nsec / 1'000'000;
    buffer << std::setw(2) << std::setfill('0') << h;
    buffer << ':';
    buffer << std::setw(2) << std::setfill('0') << m;
    buffer << ':';
    buffer << std::setw(2) << std::setfill('0') << s;
    buffer << '.';
    buffer << std::setw(3) << std::setfill('0') << ms;
    buffer << '|';
    buffer << 't' << std::this_thread::get_id();
    buffer << '|';
    buffer << ' ' << content;
    buffer << '\n';
    return buffer.str();
}

void writeLog(logger::Level level, const LogDebugInfo &info, const std::string &content) {
    static std::mutex g_writeLogMutex;

    std::string data = makeContent(level, info, content);

    std::lock_guard<std::mutex> lock(g_writeLogMutex);

    // 不要使用类似 std::cout / std::cerr 这类由其它库定义的全局对象记录日志
    // 因为记录日志可能发生于进入 main 函数之前，无法确保全局对象的状态

    if (g_writer) {
        g_writer->write(data.data(), data.size());
#ifndef NDEBUG
        std::fwrite(data.data(), 1, data.size(), stdout);
#endif
        if (shouldFlush(level)) {
            g_writer->flush();
        }
    } else {
        std::fwrite(data.data(), 1, data.size(), stdout);
    }
}

void LogLine::printPath(const fs::path &p) {
    buffer_ << p.generic_string();
}

namespace logger::control
{

static const char *const kDefaultBaseName = "abc-server-log";

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
}

} // namespace logger::control
