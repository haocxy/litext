#pragma once

#include <vector>
#include <tuple>
#include <ostream>

#include "doc/open_info.h"


class CmdOptImpl;

class CmdOpt {
public:
    CmdOpt(int argc, char *argv[]);

    ~CmdOpt();

    bool needHelp() const;

    void help(std::ostream &out) const;

    const std::vector<doc::OpenInfo> &files() const;

    const std::string logLevel() const;

private:
    CmdOptImpl *impl_ = nullptr;
};


