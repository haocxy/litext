#pragma once

#include <string>
#include <vector>
#include <optional>

#include "doc/open_info.h"


class InitInfo {
public:
    InitInfo() {}

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

    const std::optional<fs::path> &styleSheetFile() const {
        return styleSheetFile_;
    }

    void setStyleSheetFile(const fs::path &file) {
        styleSheetFile_ = file;
    }

private:
    std::string logLevel_;
    bool shouldLogToStd_ = false;
    bool shouldStartAsServer_ = false;
    std::vector<doc::OpenInfo> openInfos_;
    std::optional<fs::path> styleSheetFile_;
};
