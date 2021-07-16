#pragma once

#include "util/fs.h"

#include "logger-level.h"


namespace logger::control
{

class Option
{
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

private:
    Level level_ = Level::Info;
    fs::path dir_;
    std::string basename_;
};

void init(const Option &opt);

} // namespace logger::control
