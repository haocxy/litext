#include "logger-writer.h"

#include <sstream>

#include "util/timeutil.h"


namespace logger
{

Writer::Writer(const fs::path &dir, const std::string &basename)
    : base_(fs::absolute(dir / basename).normalize()) {

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

    const ptime now = posix_time::microsec_clock::local_time();
    tomorrowStart_ = ptime(pdate(now.date().day_count() + 1));
}

static std::string makeFilePath(const std::string &basePath, const ptime &now) {
    std::ostringstream ss;
    ss << basePath << '-';
    const pdate d = now.date();
    ss << std::setw(4) << std::setfill('0') << d.year();
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << d.month().as_number();
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << d.day().as_number();
    ss << '_';
    const auto t = now.time_of_day();
    ss << std::setw(2) << std::setfill('0') << t.hours();
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << t.minutes();
    ss << '-';
    ss << std::setw(2) << std::setfill('0') << t.seconds();
    ss << ".log";
    return ss.str();
}

void Writer::write(const void *data, size_t len) {
    if (!data || len == 0) {
        return;
    }
    const ptime now = posix_time::second_clock::local_time();
    if (now >= tomorrowStart_) {
        out_.close();
        tomorrowStart_ = ptime(pdate(tomorrowStart_.date().day_count() + 1));
    }
    if (!out_) {
        out_.open(makeFilePath(base_.generic_string(), now), std::ios::binary);
    }
    out_.write(reinterpret_cast<const char *>(data), len);
}



} // namespace logger
