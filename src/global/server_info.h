#pragma once

#include <string>
#include <sstream>

#include "core/basetype.h"
#include "core/fs.h"
#include "charset/to_utf32.h"


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
        std::string u8s = line.substr(sepLoc + 1);
        std::u32string u32s = charset::toUTF32(Charset::UTF_8, u8s.data(), u8s.size());
        return ServerInfo(port, fs::path(u32s));
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
