#include "logger-writer.h"

#include <cstring>
#include <sstream>


namespace logger
{

static void safeLocalTime(std::tm &tm, std::time_t sec) {
#ifdef WIN32
    localtime_s(&tm, &sec);
#else
    localtime_r(&sec, &tm);
#endif
}

Writer::Writer(const fs::path &dir, const std::string &basename)
    : base_(fs::absolute(dir / basename).lexically_normal()) {

    if (dir.empty()) {
        throw PathCheckError("dir is empty string");
    }

    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    } else {
        if (!fs::is_directory(dir)) {
            std::ostringstream ss;
            ss << "logger::Writer construct error: "
                << "[" << dir.generic_string() << "]"
                << "exists but not directory";
            throw PathCheckError(ss.str().c_str());
        }
    }

    if (fs::exists(base_)) {
        std::ostringstream ss;
        ss << "logger::Writer construct error: "
            << "base path [" << base_.generic_string() << "]"
            << "already exists";
        throw PathCheckError(ss.str().c_str());
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
