#pragma once

#include <stdexcept>
#include <string>
#include <fstream>

#include "util/fs.h"
#include "util/timeutil.h"
#include "util/ptime.h"


namespace logger
{

class Writer
{
public:

    class PathCheckError : public std::logic_error
    {
    public:
        PathCheckError(const char *msg) : std::logic_error(msg) {}
        virtual ~PathCheckError() {}
    };

    Writer(const fs::path &dir, const std::string &basename);

    void write(const void *data, size_t len);

    void flush() {
        out_.flush();
    }

private:
    const fs::path base_;
    std::ofstream out_;
    ptime tomorrowStart_;
};

} // namespace logger
