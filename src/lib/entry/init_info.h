#pragma once

#include <string>
#include <vector>

#include "doc/open_info.h"


class InitInfo {
public:
    InitInfo() {}

    const std::string &arg0() const {
        return arg0_;
    }

    void setArg0(const char *arg0) {
        arg0_ = arg0;
    }

    const std::string &logLevel() const {
        return logLevel_;
    }

    void setLogLevel(const std::string &logLevel) {
        logLevel_ = logLevel;
    }

    bool shouldLogToStd() const {
        return shouldLogToStd_;
    }

    void setShouldLogToStd(bool b) {
        shouldLogToStd_ = b;
    }

    bool shouldStartAsServer() const {
        return shouldStartAsServer_;
    }

    void setShouldStartAsServer(bool b) {
        shouldStartAsServer_ = b;
    }

    void addOpenInfo(const fs::path &file, long long row) {
        openInfos_.push_back(doc::OpenInfo(file, row));
    }

    const std::vector<doc::OpenInfo> &openInfos() const {
        return openInfos_;
    }

private:
    std::string arg0_;
    std::string logLevel_;
    bool shouldLogToStd_ = false;
    bool shouldStartAsServer_ = false;
    std::vector<doc::OpenInfo> openInfos_;
};
