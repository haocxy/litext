#pragma once

#include <vector>
#include <tuple>
#include <ostream>

#include "core/logger.h"
#include "doc/open_info.h"


class CmdOptImpl;

class CmdOpt {
public:
    CmdOpt(int argc, char *argv[]);

    ~CmdOpt();

    bool needHelp() const;

    void help(std::ostream &out) const;

    const std::vector<doc::OpenInfo> &files() const;

    logger::Level logLevel() const;

    bool shouldLogToStdout() const;

private:
    CmdOptImpl *impl_ = nullptr;
};


