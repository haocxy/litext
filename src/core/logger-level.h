#pragma once

namespace logger
{

enum class Level
{
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

} // namespace logger
