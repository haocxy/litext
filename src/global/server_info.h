#pragma once

#include <string>
#include <sstream>

#include "core/basetype.h"
#include "core/fs.h"


class ServerInfo {
public:
    ServerInfo() {}

    ServerInfo(u16 port, const fs::path &exePath)
        : port_(port), exePath_(exePath) {}

    ServerInfo(u16 port, fs::path &&exePath)
        : port_(port), exePath_(std::move(exePath)) {}

    std::string toLine() {
        std::ostringstream ss;
        ss << port_ << Sep << exePath_.generic_u8string();
        return ss.str();
    }

    static ServerInfo fromLine(const std::string &line) {
        const i64 sepLoc = line.find(Sep);
        std::istringstream ss(line.substr(0, sepLoc));
        u16 port = 0;
        ss >> port;
        u8str u8s(line.substr(sepLoc + 1));
        return ServerInfo(port, static_cast<std::u32string &&>(u32str(u8s)));
    }

    u16 port() const {
        return port_;
    }

    const fs::path &exePath() const {
        return exePath_;
    }

private:
    static const char Sep = ',';
    u16 port_ = 0;
    fs::path exePath_;
};
