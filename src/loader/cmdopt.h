#pragma once

#include <vector>
#include <tuple>
#include <string>
#include <filesystem>

#include "doc/open_info.h"


class CmdOptImpl;

class CmdOpt {
public:
    CmdOpt(int argc, char *argv[]);

    ~CmdOpt();

    bool needHelp() const;

    void help(std::ostream &out) const;

    const std::vector<std::pair<std::filesystem::path, int64_t>> &files() const;

    const char *logLevel() const;

    bool shouldLogToStdout() const;

    bool hasNonSingle() const;

private:
    CmdOptImpl *impl_ = nullptr;
};


