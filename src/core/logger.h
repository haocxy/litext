#pragma once

#include <string>
#include <sstream>
#include <fstream>

#include "fs.h"


namespace logger
{

enum class Level {
    All,
    Debug,
    Info,
    Warn,
    Error,
    Off,
};

inline const char *tostr(Level level) {
    switch (level) {
    case Level::Debug:
        return "d";
    case Level::Info:
        return "i";
    case Level::Warn:
        return "w";
    case Level::Error:
        return "e";
    default:
        return "?";
    }
}

bool shouldLog(logger::Level level);

class Writer {
public:

    Writer(const fs::path &dir, const std::string &basename);

    void write(const void *data, size_t len);

    void flush() {
        out_.flush();
    }

private:
    const fs::path base_;
    std::ofstream out_;
    std::tm lastTime_;
};

} // namespace logger



namespace logger::control
{

class Option {
public:
    Option() {}

    Level level() const {
        return level_;
    }

    void setLevel(Level level) {
        level_ = level;
    }

    void setLevel(const std::string &str);

    const fs::path &dir() const {
        return dir_;
    }

    void setDir(const fs::path &dir) {
        dir_ = dir;
    }

    const std::string &basename() const {
        return basename_;
    }

    void setBasename(const std::string &basename) {
        basename_ = basename;
    }

    bool isAlwaysFlush() const {
        return alwaysFlush_;
    }

    void setAlwaysFlush(bool alwaysFlush) {
        alwaysFlush_ = alwaysFlush;
    }

private:
    Level level_ = Level::Info;
    fs::path dir_;
    std::string basename_;
    bool alwaysFlush_ = false;
};

void init(const Option &opt);

}



class LogDebugInfo
{
public:
    LogDebugInfo(const char *file, int32_t line)
        : file_(file), line_(line) {
    }

    const char *file() const {
        return file_;
    }

    int32_t line() const {
        return line_;
    }

private:
    const char *file_;
    const int32_t line_;
};

void writeLog(logger::Level level, const LogDebugInfo &info, const std::string &content);

class LogLine
{
public:
    LogLine(logger::Level level, const char *file, int32_t line)
        : shouldLog_(logger::shouldLog(level))
        , level_(level)
        , debugInfo_(file, line) { }

    ~LogLine() {
        if (shouldLog_) {
            writeLog(level_, debugInfo_, buffer_.str());
        }
    }

    template <typename T>
    LogLine &operator<<(const T &obj) {
        if (shouldLog_) {
            buffer_ << obj;
        }
        return *this;
    }

    LogLine &operator<<(const char *str) {
        if (shouldLog_) {
            if (str) {
                buffer_ << str;
            } else {
                buffer_ << "(nullptr)";
            }
        }
        return *this;
    }

    LogLine &operator<<(const fs::path &p) {
        if (shouldLog_) {
            printPath(p);
        }
        return *this;
    }

private:
    void printPath(const fs::path &p);

private:
    const bool shouldLog_;
    const logger::Level level_;
    const LogDebugInfo debugInfo_;
    std::ostringstream buffer_;
};

#define LOGD (LogLine(logger::Level::Debug, __FILE__, __LINE__))
#define LOGI (LogLine(logger::Level::Info, __FILE__, __LINE__))
#define LOGW (LogLine(logger::Level::Warn, __FILE__, __LINE__))
#define LOGE (LogLine(logger::Level::Error, __FILE__, __LINE__))
