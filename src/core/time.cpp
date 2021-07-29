#include "time.h"

#include <string>
#include <sstream>
#include <map>
#include <vector>


namespace TimeUtil
{

static const std::map<std::string, int> gMapMmmToNumber{
    {"Jan",1},{"Feb",2},{"Mar",3},
    {"Apr",4},{"May",5},{"Jun",6},
    {"Jul",7},{"Aug",8},{"Sep",9},
    {"Oct",10},{"Nov",11},{"Dec",12}
};

std::chrono::system_clock::time_point compileTime(const char *dateByMacro, const char *timeByMacro)
{
    std::string date(dateByMacro);

    std::istringstream datess(date);
    std::string Mmm;
    int dd = 0;
    int yyyy = 0;
    datess >> Mmm >> dd >> yyyy;

    std::string time(timeByMacro);
    std::vector<std::string> parts;
    std::string part;
    for (char c : time) {
        if (c != ':') {
            part.push_back(c);
        } else {
            parts.push_back(std::move(part));
        }
    }
    if (!part.empty()) {
        parts.push_back(part);
    }

    const int hh = std::stoi(parts[0]);
    const int mm = std::stoi(parts[1]);
    const int ss = std::stoi(parts[2]);

    std::tm tm{};
    tm.tm_year = yyyy - 1900;
    tm.tm_mon = gMapMmmToNumber.find(Mmm)->second - 1;
    tm.tm_mday = dd;
    tm.tm_hour = hh;
    tm.tm_min = mm;
    tm.tm_sec = ss;
    tm.tm_isdst = 0;

    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

}
