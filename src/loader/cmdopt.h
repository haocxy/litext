#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <optional>
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

    std::optional<fs::path> styleSheetFile() const;

private:
    CmdOptImpl *impl_ = nullptr;
};


