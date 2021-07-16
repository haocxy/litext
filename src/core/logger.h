#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "util/fs-declare.h"

#include "logger-level.h"


namespace logger
{

bool shouldLog(logger::Level level);

} // namespace logger

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

    template <typename E>
    LogLine &operator<<(const std::vector<E> &vec) {
        if (shouldLog_) {
            buffer_ << '[';
            bool first = true;
            for (const E &e : vec) {
                if (!first) {
                    buffer_ << ',';
                }
                buffer_ << e;
                first = false;
            }
            buffer_ << ']';
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
