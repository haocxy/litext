#include "logger-control.h"

namespace logger::control
{

static Level toLevel(const std::string &str) {
    if (str.find("a") == 0 || str == "*") {
        return Level::All;
    }
    if (str == "off" || str == "-" || str == "x") {
        return Level::Off;
    }
    if (str.find("d") == 0 || str.find("D") == 0) {
        return Level::Debug;
    }
    if (str.find("i") == 0 || str.find("I") == 0) {
        return Level::Info;
    }
    if (str.find("e") == 0 || str.find("E") == 0) {
        return Level::Error;
    }
    return Level::Info;
}

void Option::setLevel(const std::string &str) {
    level_ = toLevel(str);
}

} // namespace logger::control
